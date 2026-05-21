// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/DeployableTurret/DeployableTurret.h"
#include "Kismet/GameplayStatics.h"


UComplyAbilitySystemComponent::UComplyAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UComplyAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UComplyAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UComplyAbilitySystemComponent::Server_PlaceTurret_Implementation(FVector SpawnLocation, FRotator SpawnRotation,
	TSubclassOf<ADeployableTurret> TurretClass, TSubclassOf<UGameplayEffect> InDamageEffectClass,
	FGameplayTag InDamageTypeTag, float InDamage, float InLifeSpan)
{
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ADeployableTurret* Turret = GetWorld()->SpawnActorDeferred<ADeployableTurret>(TurretClass, SpawnTransform, GetOwnerActor(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Turret)
	{
		Turret->Damage = InDamage;
		Turret->SetLifeSpan(InLifeSpan);
		Turret->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		Turret->DamageEffectClass = InDamageEffectClass;
		Turret->DamageTypeTag = InDamageTypeTag;

		UGameplayStatics::FinishSpawningActor(Turret, SpawnTransform);
	}
}

