// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/AbilityActors/DecoyGrenade/DecoyGrenade.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Disruptor::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	Super::ThrowOnServer(LaunchVelocity, SpawnPosition);
	
	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnPosition);
	
	ADecoyGrenade* DecoyGrenade = GetWorld()->SpawnActorDeferred<ADecoyGrenade>(
		ThrowableActorClass, 
		SpawnTransform, 
		GetOwningActorFromActorInfo(), 
		GetAvatarActorFromActorInfo()->GetInstigator(), 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (DecoyGrenade)
	{
		DecoyGrenade->PullRadius = PullRadius;
		DecoyGrenade->DecoyGrenadeLifetime = DecoyGrenadeLifetime;
		DecoyGrenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		DecoyGrenade->DamageEffectClass = DamageEffectClass;
		DecoyGrenade->DamageTypeTag = DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = LaunchVelocity.GetClampedToMaxSize(ThrowSpeed);
		DecoyGrenade->LaunchVelocity = SafeLaunchVelocity;

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
