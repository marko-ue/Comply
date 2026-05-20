// Copyright © 2026 Marko. All rights reserved.


#include "Actors/DeployableTurret/DeployableTurret.h"
#include <Interface/Enemy/EnemyInterface.h>
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"


ADeployableTurret::ADeployableTurret()
{
	PrimaryActorTick.bCanEverTick = true;

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
		FRotator LerpRotation = FMath::RInterpTo(GetActorRotation(), ToTargetRotation, DeltaTime, 15.f);
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
		if (FVector::DotProduct(GetActorForwardVector(), DirectionToTarget) >= 0.342f)
		{
			CurrentTarget = Target;
			Fire(Target);
			return;
		}
	}

	CurrentTarget = nullptr; // Clear when no target in cone
}

void ADeployableTurret::Fire(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, SourceASC->MakeEffectContext());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageTypeTag, Damage);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}



