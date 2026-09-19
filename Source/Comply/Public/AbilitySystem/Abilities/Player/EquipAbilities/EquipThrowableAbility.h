// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/ComplyAbilityBase.h"

#include "EquipThrowableAbility.generated.h"

/**
 * Blocks activation of all abilities except the throwable ability.
 */
UCLASS()
class COMPLY_API UEquipThrowableAbility : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	UEquipThrowableAbility();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
