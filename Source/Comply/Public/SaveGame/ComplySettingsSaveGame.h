// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ComplySettingsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UComplySettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	// --- Player Settings ---
	UPROPERTY(SaveGame)
	float SFXVolume        = 1.f;
	UPROPERTY(SaveGame)
	float MusicVolume      = 1.f;
	UPROPERTY(SaveGame)
	float AmbienceVolume   = 1.f;
	UPROPERTY(SaveGame)
	float LookSensitivity  = 1.f;
	UPROPERTY(SaveGame)
	bool bInvertY          = false;
	UPROPERTY(SaveGame)
	bool bShowHUD          = true;
	UPROPERTY(SaveGame)
	float CrosshairSize    = 1.f;
	UPROPERTY(SaveGame)
	float CrosshairOpacity = 1.f;
	UPROPERTY(SaveGame)
	FLinearColor CrosshairColor = FLinearColor::White;
};
