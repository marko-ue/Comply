// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "ComplyAbilityData.generated.h"

class UGameplayEffect;

/**
 * Data asset containing general ability information.
 */
UCLASS()
class COMPLY_API UComplyAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> CostEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> CooldownEffect;
};
