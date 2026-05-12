// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemGlobals.h"
#include "AbilitySystem/ComplyAbilityTypes.h"

FGameplayEffectContext* UComplyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FComplyGameplayEffectContext();
}
