// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Enemies/Robot/Attack_Robot.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"

UAttack_Robot::UAttack_Robot()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = ComplyTags::Events::Event_MeleeAttackRobot;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UAttack_Robot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return; 
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData && TriggerEventData->Target)
	{
		TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	}
	
	UAbilityTask_PlayMontageAndWait* AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage, 1.f, NAME_None, true);
	AttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
	AttackMontageTask->ReadyForActivation();

	if (TargetActor)
	{
		CauseDamage(TargetActor, Damage.GetValueAtLevel(GetAbilityLevel()));
	}
}

void UAttack_Robot::OnAttackAnimationFinished()
{
	Super::OnAttackAnimationFinished();
}

void UAttack_Robot::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
