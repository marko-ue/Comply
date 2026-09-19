// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"

// UE
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"

#include "ComplyEnemyDamageData.generated.h"

class UGameplayEffect;

/**
 * Data asset used for enemy damage.
 */
UCLASS()
class COMPLY_API UComplyEnemyDamageData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
