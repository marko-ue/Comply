// Copyright © 2026 Marko. All rights reserved.


#include "Actors/DeployableTurret/DeployableTurret.h"
#include <Interface/Enemy/EnemyInterface.h>
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"


ADeployableTurret::ADeployableTurret()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TurretMesh = CreateDefaultSubobject<USkeletalMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(RootComponent);

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	SphereComp->SetupAttachment(RootComponent);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComp->SetupAttachment(RootComponent);
}

UAbilitySystemComponent* ADeployableTurret::GetAbilitySystemComponent() const
{
	return ASC;
}

void ADeployableTurret::BeginPlay()
{
	Super::BeginPlay();
	
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
		// Only fire at enemies <= 70 degrees around the turret
		FVector DirectionToTarget = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		if (FVector::DotProduct(GetActorForwardVector(), DirectionToTarget) >= 0)
		{
			FHitResult Hit;
			FVector TraceStart = GetActorLocation() + FVector(0.f, 0.f, 50.f);
			FVector TraceEnd = Target->GetActorLocation() + FVector(0.f, 0.f, 50.f);
			bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);
			if (bBlocked) continue; // a wall is in the way
			
			CurrentTarget = Target;
			Fire(Target);
			return;
		}
	}

	CurrentTarget = nullptr; // Clear when no target in cone
}

void ADeployableTurret::Fire(AActor* TargetActor) const
{
	if (!SourceASC || !DamageEffectClass) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, SourceASC->MakeEffectContext());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypeTag, Damage);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
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
