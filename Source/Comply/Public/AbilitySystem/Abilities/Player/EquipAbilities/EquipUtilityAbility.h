// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ComplyAbilityBase.h"
#include "EquipUtilityAbility.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UEquipUtilityAbility : public UComplyAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
