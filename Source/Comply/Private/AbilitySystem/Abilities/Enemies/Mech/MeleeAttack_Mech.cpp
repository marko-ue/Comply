// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Enemies/Mech/MeleeAttack_Mech.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystem/ComplyTags.h"

UMeleeAttack_Mech::UMeleeAttack_Mech()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = ComplyTags::Events::Event_MeleeAttackRobot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UMeleeAttack_Mech::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return; 
	
	if (TriggerEventData && TriggerEventData->Target)
	{
		TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	}

	UAbilityTask_PlayMontageAndWait* AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage, 1.f, NAME_None, true);
	AttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
	AttackMontageTask->ReadyForActivation();
	
	FComplyGameplayEffectContext* Context = new FComplyGameplayEffectContext();
	
	if (TargetActor)
	{
		CauseDamage(TargetActor, Damage.GetValueAtLevel(GetAbilityLevel()));
	}
}

void UMeleeAttack_Mech::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UMeleeAttack_Mech::OnAttackAnimationFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
