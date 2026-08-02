// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "DisruptorData.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UDisruptorData : public UComplyPlayerData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 ShotgunMaxAmmo = 30;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 ShotgunMaxReserveAmmo = 270;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 DecoyGrenadeMaxCharges = 3;
};
