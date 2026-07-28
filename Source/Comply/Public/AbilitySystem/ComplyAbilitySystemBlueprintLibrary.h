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
	
	// Helper function that outputs a trace start, end and direction
	UFUNCTION(Category = "Trace")
	static bool GetCrosshairTraceStartEnd(const UObject* WorldContextObject, const AActor* Avatar, const float TraceLength, FVector& OutStart, FVector& OutEnd, FVector& OutDirection);
};
