// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "UObject/Interface.h"

// Comply
#include "Enums/EWeaponSlot.h"

#include "PlayerInterface.generated.h"

UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface used to differentiate players from other characters, and used for weapon equipping.
 */
class COMPLY_API IPlayerInterface
{
	GENERATED_BODY()

public:
	// The mesh for the relevant weapon (slot) will be returned
	virtual UStaticMesh* GetMeshForSlot(EWeaponSlot Slot) = 0;
	
	// The scale for the mesh for the relevant weapon (slot) will be returned
	virtual FVector GetScaleForSlot(EWeaponSlot Slot) = 0;
	
	// The function that will be called by equip abilities whenever a weapon is equipped
	virtual void OnWeaponEquipped(EWeaponSlot Slot) = 0;
	
	virtual void ClearEquippedWeapon() = 0;
};
