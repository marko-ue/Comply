// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyWeaponData.h"
#include "ShotgunWeaponData.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UShotgunWeaponData : public UComplyWeaponData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shotgun Settings")
	int32 NumberOfPellets = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shotgun Settings")
	float SpreadAngle = 10.f;
};
