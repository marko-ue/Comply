// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageAbilityBase.h"
#include "AbilitySystem/Data/Enemy/Abilities/ComplyEnemyAbilityData.h"
#include "AbilitySystem/Data/Enemy/Damage/ComplyEnemyDamageData.h"
#include "Character/ComplyEnemyCharacter.h"
#include "EnemyAbilityBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UEnemyAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UFUNCTION()
	virtual void OnAttackAnimationFinished();
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
};
