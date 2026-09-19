// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/UtilityAbilityBase.h"

#include "Utility_Disruptor.generated.h"

class ABuffTotem;
class ABuffTotemPreview;

/**
 * Buff totem that buffs teammates' speed and damage if they pick up the buff by walking in its radius.
 */
UCLASS()
class COMPLY_API UUtility_Disruptor : public UUtilityAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void TraceAndSpawn() override;
	
	virtual void ConfirmPlacement() override;
};
