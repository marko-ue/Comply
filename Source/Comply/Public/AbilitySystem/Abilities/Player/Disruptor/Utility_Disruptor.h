// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Disruptor.generated.h"

class ABuffTotem;
class ABuffTotemPreview;
/**
 * 
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
