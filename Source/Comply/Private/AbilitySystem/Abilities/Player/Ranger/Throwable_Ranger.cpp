// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"

#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/PlasmaGrenade/PlasmaGrenade.h"
#include "Actors/PlasmaGrenade/PlasmaGrenadePreview.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UThrowable_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	SpawnPreview();
}

void UThrowable_Ranger::SpawnPreview()
{
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	FTransform SpawnTransform = FTransform(
		InstigatorPawn->GetActorRotation(),
		InstigatorPawn->GetActorLocation()
	);
	
	SpawnedGrenadePreview = GetWorld()->SpawnActorDeferred<APlasmaGrenadePreview>(
		GrenadePreviewActorClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	// Information needed to predict the path correctly
	SpawnedGrenadePreview->ActorsToIgnore.Add(InstigatorPawn);
	SpawnedGrenadePreview->OwningPawn = InstigatorPawn;
	SpawnedGrenadePreview->ThrowSpeed = ThrowSpeed;
	
	UGameplayStatics::FinishSpawningActor(SpawnedGrenadePreview, SpawnTransform);
}

void UThrowable_Ranger::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if (SpawnedGrenadePreview) SpawnedGrenadePreview->Destroy();
}

// This function is overridden so ability costs can be handled manually
// The charge would usually get consumed when the input is pressed, doing it manually allows the player to use all charges
bool UThrowable_Ranger::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	return true;
}

void UThrowable_Ranger::ConfirmThrow()
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostEffectClass, 1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	if (SpawnedGrenadePreview) SpawnedGrenadePreview->Destroy();
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	FTransform SpawnTransform = FTransform(
		InstigatorPawn->GetActorRotation(),
		InstigatorPawn->GetActorLocation()
	);
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(
		GrenadeActorClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	// TODO: Turn both into a scalable float to make it upgradeable
	Grenade->ExplosionRadius = 1000.f;
	Grenade->MaxDamage = 150.f;
	Grenade->SourceASC = GetAbilitySystemComponentFromActorInfo();
	Grenade->DamageEffectClass = DamageEffectClass;
	Grenade->DamageTypeTag = DamageType;
	
	FVector LaunchVelocity = GetAvatarActorFromActorInfo()->GetActorLocation().ForwardVector * ThrowSpeed;
	Grenade->ProjectileMovementComp->Velocity = LaunchVelocity;
	
	UGameplayStatics::FinishSpawningActor(Grenade, SpawnTransform);
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
