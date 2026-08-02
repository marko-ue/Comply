// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "AbilitySystem/Data/Player/Grenades/DecoyGrenadeData.h"
#include "Actors/AbilityActors/DecoyGrenade/DecoyGrenade.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Disruptor::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	checkf(GrenadeData, TEXT("GrenadeData not set on %s"), *GetName());
	
	Super::ThrowOnServer(LaunchVelocity, SpawnPosition);
	
	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnPosition);
	
	ADecoyGrenade* DecoyGrenade = GetWorld()->SpawnActorDeferred<ADecoyGrenade>(
		GrenadeData->GrenadeActorClass, 
		SpawnTransform, 
		GetOwningActorFromActorInfo(), 
		GetAvatarActorFromActorInfo()->GetInstigator(), 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (DecoyGrenade)
	{
		DecoyGrenade->GrenadeData = Cast<UDecoyGrenadeData>(GrenadeData);
		DecoyGrenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		DecoyGrenade->LaunchVelocity = LaunchVelocity.GetClampedToMaxSize(DecoyGrenade->GrenadeData->ThrowSpeed);
    
		UGameplayStatics::FinishSpawningActor(DecoyGrenade, SpawnTransform);
	}
}

void UThrowable_Disruptor::CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity,
                                        FVector SpawnPosition)
{
	const APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	ASC->Server_ThrowDecoyGrenade(GetCurrentAbilitySpecHandle(), SpawnPosition, InstigatorPawn->GetActorRotation(), LaunchVelocity);
}

FGameplayAttribute UThrowable_Disruptor::GetThrowableCurrentChargesAttribute()
{
	return UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute();
}
