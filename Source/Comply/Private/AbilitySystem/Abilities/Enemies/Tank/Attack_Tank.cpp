// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Enemies/Tank/Attack_Tank.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"

UAttack_Tank::UAttack_Tank()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = ComplyTags::Events::Event_AttackTank;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UAttack_Tank::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	
	FVector EnemyLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector EnemyForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				FVector ToPlayer = (PlayerPawn->GetActorLocation() - EnemyLocation);
				float Distance = ToPlayer.Size();

				if (Distance <= SweepRange)
				{
					// Sweeps from the front
					FVector ToPlayerNormalized = ToPlayer.GetSafeNormal();
					float DotProduct = FVector::DotProduct(EnemyForward, ToPlayerNormalized);

					if (DotProduct >= ConeHalfAngleDot)
					{
						CauseDamage(PlayerPawn, Damage.GetValueAtLevel(GetAbilityLevel()));
					}
				}
			}
		}
	}
}

void UAttack_Tank::OnAttackAnimationFinished()
{
	Super::OnAttackAnimationFinished();
}

void UAttack_Tank::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
