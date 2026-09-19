// Copyright © 2026 Marko. All rights reserved.

#pragma once

/**
 * Enum for different weapon slots (the different slots that can be equipped)
 */
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	None,
	Primary,
	Utility,
	Throwable
};