// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemies/EnemyAbilityBase.h"
#include "Attack_Tank.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UAttack_Tank : public UEnemyAbilityBase
{
	GENERATED_BODY()
	
public:
	UAttack_Tank();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void OnAttackAnimationFinished() override;
	
private:
	UPROPERTY(EditAnywhere)
	float SweepRange = 500.f;
	
	UPROPERTY(EditAnywhere)
	float ConeHalfAngleDot = 0.0f;
};
