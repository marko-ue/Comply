// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystemGlobals.h"

#include "ComplyAbilitySystemGlobals.generated.h"

/**
 * Holds global data for the ability system. 
 */
UCLASS()
class COMPLY_API UComplyAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
	// Allows the custom effect context to be used when applying gameplay effects
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
