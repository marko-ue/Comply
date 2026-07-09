// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/DecoyGrenade/DecoyGrenade.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Comply.h"
#include "AbilitySystem/ComplyTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"


ADecoyGrenade::ADecoyGrenade()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>("GrenadeMesh");
	SetRootComponent(GrenadeMesh);
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->bShouldBounce = true;
	ProjectileMovementComp->Bounciness = 0.1f;
	ProjectileMovementComp->Friction = 0.8f;
	ProjectileMovementComp->BounceAdditionalIterations = 1;
	ProjectileMovementComp->SetUpdatedComponent(GrenadeMesh);
	
	RotatingMovementComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComp"));
	RotatingMovementComp->RotationRate = FRotator(200.f, 150.f, 0.f);
	
	ProjectileMovementComp->OnProjectileStop.AddDynamic(this, &ADecoyGrenade::OnGrenadeLanded);
}

void ADecoyGrenade::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, LaunchVelocity);
}

void ADecoyGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	// Movement shouldn't be replicated as clients now have a local projectile for smoothness
	SetReplicateMovement(false);
	
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &ThisClass::Explode, 4.f, false);
	}
	
	ProjectileMovementComp->Velocity = LaunchVelocity; // Sets the velocity of the grenade throw
}

void ADecoyGrenade::OnRep_LaunchVelocity()
{
	ProjectileMovementComp->Velocity = LaunchVelocity;
}

void ADecoyGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADecoyGrenade::Explode()
{
	if (!HasAuthority()) return;

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(PullRadius);
	GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!TargetASC) continue;
		
		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ApplyDistractedEffectClass, 1.f, SourceASC->MakeEffectContext());
		FActiveGameplayEffectHandle Handle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		
		// All ASCs affected by this effect and all active effects are stored in arrays so they can be accessed and cleared when the grenade is destroyed
		AffectedASCs.Add(TargetASC);
		DistractedEffectHandles.Add(Handle);
		
		// When the decoy grenade explodes, the blackboard key responsible for moving the enemy to a distraction is set
		if (AAIController* AIC = Cast<AAIController>(HitActor->GetInstigatorController()))
		{
			UBlackboardComponent* BB = AIC->GetBlackboardComponent();
			if (BB)
			{
				// Set the distraction location to this actor's location upon the explosion
				// All enemies will now go to this location
				// Also clear TargetActor so the only option for affected enemies is to go to the distraction location
				BB->SetValueAsVector("DistractionLocation", GetActorLocation());
				BB->ClearValue("TargetActor");
				AffectedBBs.Add(BB);
			}
		}
		
		GrenadeMesh->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);
		GrenadeMesh->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = GetActorLocation();
	CueParams.EffectCauser = this;
	if (SourceASC)
	{
		SourceASC->ExecuteGameplayCue(ComplyTags::GameplayCues::ExplodeGrenade, CueParams);
	}
	
	// The grenade will get destroyed after its lifetime passes, and it will stop pulling enemies at this point
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, FTimerDelegate::CreateLambda([this]()
	{ Destroy(); }), DecoyGrenadeLifetime, false);
}

void ADecoyGrenade::OnGrenadeLanded(const FHitResult& ImpactResult)
{
	RotatingMovementComp->RotationRate = FRotator::ZeroRotator;
}

void ADecoyGrenade::Destroyed()
{
	for (int32 i = 0; i < AffectedASCs.Num(); i++)
	{
		// Remove the distracted effect from all enemies as the grenade is now destroyed
		if (AffectedASCs[i]) AffectedASCs[i]->RemoveActiveGameplayEffect(DistractedEffectHandles[i]);
	}
	
	for (int32 i =0; i < AffectedBBs.Num(); i++)
	{
		// Clear the value of DistractionLocation so enemies can go back to chasing the player
		if (AffectedBBs[i]) AffectedBBs[i]->ClearValue("DistractionLocation");
	}
	
	Super::Destroyed();
}
