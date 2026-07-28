// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemies/EnemyAbilityBase.h"
#include "ChargeAttack_Tank.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UChargeAttack_Tank : public UEnemyAbilityBase
{
	GENERATED_BODY()
	
public:
	UChargeAttack_Tank();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void OnAttackAnimationFinished() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Attack Settings")
	float ChargeSpeed = 1200.f;

	float DefaultMaxFlySpeed;
	float DefaultBrakingDeceleration;

	void RestoreMovement(const FGameplayAbilityActorInfo* ActorInfo) const;
};
