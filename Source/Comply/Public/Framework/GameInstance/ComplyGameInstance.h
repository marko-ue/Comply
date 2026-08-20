// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ComplyGameInstance.generated.h"

class AComplyPlayerCharacter;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Character selections are stored in the GameInstance rather than PlayerState
	// because seamless travel player migration is unreliable for the listen server host,
	// whose controller is recreated via PostLogin rather than migrated via InitSeamlessTravelPlayer.
	// GameInstance persists across all travels and is accessible to all server-side systems
	UPROPERTY()
	TMap<FString, TSubclassOf<AComplyPlayerCharacter>> PlayerCharacterSelections;
	
	// This replicated variable is used for clients to check if friendly fire is enabled which persists through travel
	// The game mode uses this variable to set its friendly fire value after traveling to new maps and at the start of the game
	UPROPERTY(Replicated)
	bool bFriendlyFire = false;
	
	// --- Player Settings ---
	UPROPERTY()
	float SFXVolume        = 1.f;
	UPROPERTY()
	float MusicVolume      = 1.f;
	UPROPERTY()
	float AmbienceVolume   = 1.f;
	UPROPERTY()
	float LookSensitivity  = 1.f;
	UPROPERTY()
	bool bInvertY          = false;
	UPROPERTY()
	bool bShowHUD          = true;
	UPROPERTY()
	float CrosshairSize    = 32.f;
	UPROPERTY()
	float CrosshairOpacity = 1.f;
	UPROPERTY()
	FLinearColor CrosshairColor = FLinearColor::White;
	
	// Functions for saving and loading game settings that players set
	void SaveSettings();
	void LoadSettings();
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundMix> MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundClass> SFXSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundClass> MusicSoundClass;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundClass> AmbienceSoundClass;
	
	void ApplyAudioSettings() const;
};
