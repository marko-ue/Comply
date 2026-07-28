// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/AbilityActors/PlasmaGrenade/PlasmaGrenade.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Ranger::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	Super::ThrowOnServer(LaunchVelocity, SpawnPosition);
	
	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnPosition);
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(
		ThrowableActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		GetAvatarActorFromActorInfo()->GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Grenade)
	{
		Grenade->ExplosionRadius = ExplosionRadius;
		Grenade->MaxDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		Grenade->SourceASC = GetAbilitySystemComponentFromActorInfo();
		Grenade->DamageEffectClass = DamageEffectClass;
		Grenade->DamageTypeTag = DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = LaunchVelocity.GetClampedToMaxSize(ThrowSpeed);
		Grenade->LaunchVelocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(Grenade, SpawnTransform);
	}
}

void UThrowable_Ranger::CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition)
{
	const APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	ASC->Server_ThrowPlasmaGrenade(GetCurrentAbilitySpecHandle(), SpawnPosition, InstigatorPawn->GetActorRotation(), LaunchVelocity);
}

FGameplayAttribute UThrowable_Ranger::GetThrowableCurrentChargesAttribute()
{
	return UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute();
}
