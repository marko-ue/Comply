// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"

void UPrimary_Disruptor::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	// TODO: Apply cosmetic effects (muzzle flash, tracer, sound) that do no damage to make the shot feel responsive
	
	Fire();
}

bool UPrimary_Disruptor::Fire()
{
	if (!Super::Fire()) return false;
	
	PlayAnimationBasedOnState();
	
	return true;
}

void UPrimary_Disruptor::PlayAnimationBasedOnState()
{
	Super::PlayAnimationBasedOnState();
}

void UPrimary_Disruptor::PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay)
{
	Super::PlayMontageAndBindDelegates(AnimationToPlay);
}

void UPrimary_Disruptor::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UPrimary_Disruptor::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPrimary_Disruptor::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	GetAbilitySystemComponentFromActorInfo()->RemoveReplicatedLooseGameplayTag(ComplyTags::States::State_Firing);
}
