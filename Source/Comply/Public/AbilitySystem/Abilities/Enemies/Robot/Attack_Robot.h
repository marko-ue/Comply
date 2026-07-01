// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/Enemies/EnemyAbilityBase.h"
#include "Attack_Robot.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UAttack_Robot : public UEnemyAbilityBase
{
	GENERATED_BODY()
	
public:
	UAttack_Robot();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void OnAttackAnimationFinished() override;

};
