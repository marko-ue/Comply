// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "AbilitySystem/Data/Player/Grenades/PlasmaGrenadeData.h"
#include "Actors/AbilityActors/PlasmaGrenade/PlasmaGrenade.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Ranger::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	checkf(GrenadeData, TEXT("GrenadeData not set on %s"), *GetName());
	
	Super::ThrowOnServer(LaunchVelocity, SpawnPosition);
	
	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnPosition);
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(
		GrenadeData->GrenadeActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		GetAvatarActorFromActorInfo()->GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Grenade)
	{
		Grenade->GrenadeData = Cast<UPlasmaGrenadeData>(GrenadeData);
		Grenade->SourceASC = GetAbilitySystemComponentFromActorInfo();
		Grenade->LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(Grenade->GrenadeData->ThrowSpeed);

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
