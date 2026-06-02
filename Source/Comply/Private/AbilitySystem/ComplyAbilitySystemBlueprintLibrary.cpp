// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/ComplyAbilityTypes.h"

FGameplayAbilityTargetDataHandle UComplyAbilitySystemBlueprintLibrary::GetShotgunTargetData(
	const FGameplayCueParameters& Parameters)
{
	const FComplyGameplayEffectContext* Context = static_cast<const FComplyGameplayEffectContext*>(Parameters.EffectContext.Get());
	if (Context)
	{
		return Context->ShotgunTracesTargetData;
	}
	return FGameplayAbilityTargetDataHandle();
}

int32 UComplyAbilitySystemBlueprintLibrary::GetShotgunTargetDataNum(const FGameplayAbilityTargetDataHandle& Handle)
{
	return Handle.Num();
}

