// Copyright © 2026 Marko. All rights reserved.


#include "Actors/DecoyGrenade/DecoyGrenade.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/Enemy/EnemyInterface.h"


ADecoyGrenade::ADecoyGrenade()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>("GrenadeMesh");
	SetRootComponent(GrenadeMesh);
	GrenadeMesh->SetIsReplicated(true);
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
}

void ADecoyGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &ThisClass::Explode, 4.f, false);
	}
	
	const float SpinDirection = FMath::RandBool() ? 1.f : -1.f;
	GrenadeMesh->AddTorqueInRadians(FVector(250000.f * SpinDirection, 250000.f * SpinDirection, 250000.f * SpinDirection));
}

void ADecoyGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADecoyGrenade::Destroyed()
{
	for (int32 i = 0; i < AffectedASCs.Num(); i++)
	{
		// Remove the distracted effect from all enemies as the grenade is now destroyed
		if (AffectedASCs[i]) AffectedASCs[i]->RemoveActiveGameplayEffect(DistractedEffectHandles[i]);
	}
	Super::Destroyed();
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
		
		//GrenadeMesh->SetSimulatePhysics(false);
		GrenadeMesh->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);
		GrenadeMesh->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	}
	
	// The grenade will get destroyed after its lifetime passes, and it will stop pulling enemies at this point
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, FTimerDelegate::CreateLambda([this]()
	{ Destroy(); }), DecoyGrenadeLifetime, false);
}
