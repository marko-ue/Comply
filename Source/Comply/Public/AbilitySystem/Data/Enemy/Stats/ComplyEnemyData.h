// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Data/ComplyCharacterStatData.h"

#include "ComplyEnemyData.generated.h"

/**
 * Data asset used to set enemy-specific variables.
 */
UCLASS()
class COMPLY_API UComplyEnemyData : public UComplyCharacterStatData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ArmorPenetration = 0.1f;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> EnemyHealthBarWidgetClass;
};
