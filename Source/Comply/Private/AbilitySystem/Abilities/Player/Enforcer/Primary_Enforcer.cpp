// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Enforcer/Primary_Enforcer.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"


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

void UPrimary_Enforcer::PlayAnimationBasedOnState()
{
	Super::PlayAnimationBasedOnState();
}

void UPrimary_Enforcer::PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay)
{
	Super::PlayMontageAndBindDelegates(AnimationToPlay);
}

void UPrimary_Enforcer::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPrimary_Enforcer::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UPrimary_Enforcer::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	GetAbilitySystemComponentFromActorInfo()->RemoveReplicatedLooseGameplayTag(ComplyTags::States::State_Firing);
}
