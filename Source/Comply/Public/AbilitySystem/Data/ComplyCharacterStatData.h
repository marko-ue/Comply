// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComplyCharacterStatData.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyCharacterStatData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxHealth = 100.f;
};
