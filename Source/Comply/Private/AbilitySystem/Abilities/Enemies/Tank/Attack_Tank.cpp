// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Enemies/Tank/Attack_Tank.h"

#include "AbilitySystemComponent.h"
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
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return; 
	
	const AComplyEnemyCharacter* Enemy = Cast<AComplyEnemyCharacter>(GetAvatarActorFromActorInfo());
	if (!Enemy) return;
	
	checkf(Enemy->EnemyAbilityData, TEXT("EnemyAbilityData not set on %s"), *Enemy->GetName());
	checkf(Enemy->EnemyAbilityData->EnemyDamageData, TEXT("EnemyDamageData not set on %s"), *Enemy->GetName());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData && TriggerEventData->Target)
	{
		TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	}

	UAbilityTask_PlayMontageAndWait* AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage, 1.f, NAME_None, true);
	AttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
	AttackMontageTask->ReadyForActivation();
	
	const FVector EnemyLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector EnemyForward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	
	bool bHitAnything = false;

	// Collect all candidates - players + any other targetable actors in range
	TArray<AActor*> Candidates;
    
	// Players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const APlayerController* PC = It->Get())
		{
			if (APawn* PlayerPawn = PC->GetPawn())
			{
				Candidates.Add(PlayerPawn);
			}
		}
	}

	// Non-player damageables (turret, etc.)
	TArray<AActor*> OverlappingActors;
	
	if (TargetActor && !Candidates.Contains(TargetActor))
	{
		Candidates.Add(TargetActor);
	}

	for (AActor* Candidate : Candidates)
	{
		FVector ToTarget = Candidate->GetActorLocation() - EnemyLocation;
		const float Distance = ToTarget.Size();

		if (Distance <= SweepRange)
		{
			FVector ToTargetNormalized = ToTarget.GetSafeNormal();
			float DotProduct = FVector::DotProduct(EnemyForward, ToTargetNormalized);

			if (Enemy && DotProduct >= ConeHalfAngleDot)
			{
				if (!Enemy->EnemyAbilityData || !Enemy->EnemyAbilityData->EnemyDamageData) return;
				
				CauseDamage(Candidate, Enemy->EnemyAbilityData->EnemyDamageData->Damage.GetValueAtLevel(GetAbilityLevel()));
				bHitAnything = true;
			}
		}
	}
	
	// Also charge if the target actor gets hit
	if (bHitAnything)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayEventData ChargeEventData;
			ChargeEventData.Target = TargetActor;
			ASC->HandleGameplayEvent(ComplyTags::Events::Event_ChargeAttackTank, &ChargeEventData);
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
