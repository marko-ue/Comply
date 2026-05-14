// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"

#include "Actors/PlasmaGrenade/PlasmaGrenade.h"
#include "Kismet/GameplayStatics.h"

void UThrowable_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Throw();
}

void UThrowable_Ranger::Throw()
{
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	FTransform SpawnTransform = FTransform(
		InstigatorPawn->GetActorRotation(),
	InstigatorPawn->GetActorLocation() + FVector(0, 0, 50.f)
	);
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(
		GrenadeActorClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	// TODO: Turn both into a scalable float to make it upgradeable
	Grenade->ExplosionRadius = 1000.f;
	Grenade->MaxDamage = 100.f;
	Grenade->SourceASC = GetAbilitySystemComponentFromActorInfo();
	Grenade->DamageEffectClass = DamageEffectClass;
	Grenade->DamageTypeTag = DamageType;
	
	UGameplayStatics::FinishSpawningActor(Grenade, SpawnTransform);
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
