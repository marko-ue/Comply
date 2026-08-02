// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "EnforcerData.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UEnforcerData : public UComplyPlayerData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 MagnumMaxAmmo = 8;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	int32 MagnumMaxReserveAmmo = 72;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 TurretMaxCharges = 2;
};
