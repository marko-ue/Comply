// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComplyDamageCalculationConfig.generated.h"

/**
 * Data asset for all damage calculation constants
 */
UCLASS()
class COMPLY_API UComplyDamageCalculationConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Armor")
	float ArmorConstant = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Armor")
	float ArmorReductionCap = 0.75f;
};
