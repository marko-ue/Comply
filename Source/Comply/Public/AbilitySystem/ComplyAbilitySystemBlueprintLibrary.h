// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ComplyAbilitySystemBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UComplyAbilitySystemBlueprintLibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:
	/* Shotgun trace target data */
	UFUNCTION(BlueprintPure)
	static FGameplayAbilityTargetDataHandle GetShotgunTargetData(const FGameplayCueParameters& Parameters);
	
	UFUNCTION(BlueprintPure)
	static int32 GetShotgunTargetDataNum(const FGameplayAbilityTargetDataHandle& Handle);
	/* End shotgun trace target data */
	
};
