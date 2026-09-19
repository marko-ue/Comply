// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/ComplyAbilityBase.h"

#include "EquipPrimaryAbility.generated.h"

/**
 * Blocks activation of all abilities except the primary ability.
 */
UCLASS()
class COMPLY_API UEquipPrimaryAbility : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	UEquipPrimaryAbility();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
