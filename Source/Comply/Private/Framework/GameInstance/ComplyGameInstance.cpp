// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameInstance/ComplyGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SaveGame/ComplySettingsSaveGame.h"

void UComplyGameInstance::Init()
{
	Super::Init();
	
	LoadSettings();
}

void UComplyGameInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UComplyGameInstance, bFriendlyFire);
}

// Saves the new settings to the save game
void UComplyGameInstance::SaveSettings()
{
	UComplySettingsSaveGame* Save = Cast<UComplySettingsSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UComplySettingsSaveGame::StaticClass()));

	if (!Save) return;

	Save->SFXVolume        = SFXVolume;
	Save->MusicVolume      = MusicVolume;
	Save->LookSensitivity  = LookSensitivity;
	Save->bInvertY         = bInvertY;
	Save->bShowHUD         = bShowHUD;
	Save->CrosshairSize    = CrosshairSize;
	Save->CrosshairOpacity = CrosshairOpacity;
	Save->CrosshairColor   = CrosshairColor;

	UGameplayStatics::SaveGameToSlot(Save, TEXT("Settings"), 0);
}

// Loads setting from the save game
void UComplyGameInstance::LoadSettings()
{
	UComplySettingsSaveGame* Save = Cast<UComplySettingsSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("Settings"), 0));

	if (!Save) return; // no save file yet, defaults stay

	SFXVolume        = Save->SFXVolume;
	MusicVolume      = Save->MusicVolume;
	LookSensitivity  = Save->LookSensitivity;
	bInvertY         = Save->bInvertY;
	bShowHUD         = Save->bShowHUD;
	CrosshairSize    = Save->CrosshairSize;
	CrosshairOpacity = Save->CrosshairOpacity;
	CrosshairColor   = Save->CrosshairColor;
}
