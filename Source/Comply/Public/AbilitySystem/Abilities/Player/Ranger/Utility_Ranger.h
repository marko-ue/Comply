// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/UtilityAbilityBase.h"

#include "Utility_Ranger.generated.h"

class AShieldDomePreview;

/**
 * Shield dome that when placed prevents enemies from going inside, and shots and projectiles that pass through it deal extra damage.
 */
UCLASS()
class COMPLY_API UUtility_Ranger : public UUtilityAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void TraceAndSpawn() override;
	
	virtual void ConfirmPlacement() override;
};
