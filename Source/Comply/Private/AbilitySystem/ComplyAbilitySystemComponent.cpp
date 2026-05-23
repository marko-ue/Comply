// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "Actors/PlasmaGrenade/PlasmaGrenade.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/Player/Enforcer/Throwable_Enforcer.h"
#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "Actors/DeployableTurret/DeployableTurret.h"
#include "GameFramework/ProjectileMovementComponent.h"
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

void UComplyAbilitySystemComponent::Server_ThrowGrenade_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation, FVector InLaunchVelocity)
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	const UThrowable_Ranger* Ability = Cast<UThrowable_Ranger>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(Ability->GrenadeActorClass, SpawnTransform, GetOwnerActor(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Grenade)
	{
		Grenade->ExplosionRadius = Ability->ExplosionRadius;
		Grenade->MaxDamage = Ability->Damage.GetValueAtLevel(Ability->GetAbilityLevel());
		Grenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		Grenade->DamageEffectClass = Ability->DamageEffectClass;
		Grenade->DamageTypeTag = Ability->DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = InLaunchVelocity.GetClampedToMaxSize(Ability->ThrowSpeed);
		Grenade->ProjectileMovementComp->Velocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(Grenade, SpawnTransform);
	}
}

void UComplyAbilitySystemComponent::Server_PlaceTurret_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation)
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	const UThrowable_Enforcer* Ability = Cast<UThrowable_Enforcer>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ADeployableTurret* Turret = GetWorld()->SpawnActorDeferred<ADeployableTurret>(Ability->TurretActorClass, SpawnTransform, GetOwnerActor(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Turret)
	{
		Turret->Damage = Ability->Damage.GetValueAtLevel(Ability->GetAbilityLevel());
		Turret->SetLifeSpan(Ability->TurretLifetime);
		Turret->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		Turret->DamageEffectClass = Ability->DamageEffectClass;
		Turret->DamageTypeTag = Ability->DamageType;

		UGameplayStatics::FinishSpawningActor(Turret, SpawnTransform);
	}
}

