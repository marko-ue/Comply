// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "RangerData.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API URangerData : public UComplyPlayerData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 RifleMaxAmmo = 30;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 RifleMaxReserveAmmo = 270;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 PlasmaGrenadeMaxCharges = 3;
};
