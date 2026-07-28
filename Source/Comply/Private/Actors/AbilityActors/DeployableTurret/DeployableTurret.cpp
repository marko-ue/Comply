// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/DeployableTurret/DeployableTurret.h"
#include <Interface/Enemy/EnemyInterface.h>
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"


ADeployableTurret::ADeployableTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SetRootComponent(TurretMesh);

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	SphereComp->SetupAttachment(RootComponent);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComp->SetupAttachment(RootComponent);
}

void ADeployableTurret::BeginPlay()
{
	Super::BeginPlay();
	
	PlaceTurretNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PlaceTurretParticles, GetActorLocation());
	
	GetOverlappingActors(TargetsInRange);
	
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ThisClass::TryFire, .2f, true);
}

void ADeployableTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Handles interpolating rotation to the turret's current target
	if (CurrentTarget)
	{
		FRotator ToTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CurrentTarget->GetActorLocation());
		ToTargetRotation.Pitch = 0.f;
		ToTargetRotation.Roll = 0.f;
		/* Interp speed could be upgradeable in the future */
		FRotator LerpRotation = FMath::RInterpTo(GetActorRotation(), ToTargetRotation, DeltaTime, 10.f);
		SetActorRotation(LerpRotation);
	}
}

void ADeployableTurret::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UEnemyInterface>()) TargetsInRange.AddUnique(OtherActor);
}

void ADeployableTurret::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TargetsInRange.Remove(OtherActor);
}

void ADeployableTurret::TryFire()
{
	for (AActor* Target : TargetsInRange)
	{
		FHitResult Hit;
		FVector TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
		FVector TraceEnd = Target->GetActorLocation() + FVector(0.f, 0.f, 50.f);
		bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);
		if (bBlocked) continue; // A wall is in the way

		CurrentTarget = Target;
		Fire(Target);
		return;
	}

	CurrentTarget = nullptr; // Clear when no target in cone
}

void ADeployableTurret::Fire(AActor* TargetActor)
{
	if (!SourceASC || !DamageEffectClass) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, SourceASC->MakeEffectContext());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypeTag, Damage);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	
	FGameplayCueParameters CueParams;
	CueParams.Location = TargetActor->GetActorLocation();
	SourceASC->ExecuteGameplayCue(ComplyTags::GameplayCues::TurretImpact, CueParams);

	// Multicast for the turret fire sound
	Multicast_TurretFire();
}

void ADeployableTurret::Multicast_TurretFire_Implementation()
{
	if (TurretFireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TurretFireSound, GetActorLocation());
	}
}

// When the actor is destroyed, the player will be able to spawn another turret after 30 seconds
void ADeployableTurret::Destroyed()
{
	if (SourceASC)
	{
		TWeakObjectPtr<UAbilitySystemComponent> WeakASC = SourceASC;
		TSubclassOf<UGameplayEffect> EffectClass = RechargeTurretChargeClass;

		FTimerHandle RechargeTimer;
		GetWorld()->GetTimerManager().SetTimer(RechargeTimer, [WeakASC, EffectClass]()
		{
			if (WeakASC.IsValid() && EffectClass)
			{
				FGameplayEffectSpecHandle SpecHandle = WeakASC->MakeOutgoingSpec(EffectClass, 1.f, WeakASC->MakeEffectContext());
				WeakASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}, 30.f, false);
	}

	Super::Destroyed();
}

void ADeployableTurret::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// If the turret was not killed by enemies, play its death cue here. The reason for death would be turret lifetime
	if (!bWasKilledByEnemies)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		GetAbilitySystemComponent()->ExecuteGameplayCue(ComplyTags::GameplayCues::TargetableActorDeath, CueParams);
	}
	
	if (PlaceTurretNiagaraComponent)
	{
		PlaceTurretNiagaraComponent->Deactivate();
	}
	
	Super::EndPlay(EndPlayReason);
}
