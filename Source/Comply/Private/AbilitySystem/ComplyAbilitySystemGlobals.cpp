// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemGlobals.h"
#include "AbilitySystem/ComplyAbilityTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ComplyAbilitySystemGlobals)

FGameplayEffectContext* UComplyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FComplyGameplayEffectContext();
}
