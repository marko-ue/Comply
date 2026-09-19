// Copyright © 2026 Marko. All rights reserved.

#include "AbilitySystem/Abilities/Player/Enforcer/Primary_Enforcer.h"

// UE
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Primary_Enforcer)

void UPrimary_Enforcer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	// TODO: Apply cosmetic effects (muzzle flash, tracer, sound) that do no damage to make the shot feel responsive
	
	Fire();
}

bool UPrimary_Enforcer::Fire()
{
	if (!Super::Fire()) return false;
	
	PlayAnimationBasedOnState();
	
	return true;
}
