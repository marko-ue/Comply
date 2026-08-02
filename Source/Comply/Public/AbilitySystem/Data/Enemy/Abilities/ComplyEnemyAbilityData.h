// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComplyEnemyAbilityData.generated.h"

class UComplyEnemyDamageData;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyEnemyAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TObjectPtr<UComplyEnemyDamageData> EnemyDamageData;
};
