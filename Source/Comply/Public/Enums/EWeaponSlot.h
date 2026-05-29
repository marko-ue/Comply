#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	None,
	Primary,
	Utility,
	Throwable
};