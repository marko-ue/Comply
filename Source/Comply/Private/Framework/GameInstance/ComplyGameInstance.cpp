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

void UComplyGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	if (NewWorld)
	{
		// When the world changes, apply the audio settings immediately since they don't get applied at the start like the other settings
		ApplyAudioSettings();
	}
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
	Save->AmbienceVolume   = AmbienceVolume;
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
	AmbienceVolume   = Save->AmbienceVolume;
	LookSensitivity  = Save->LookSensitivity;
	bInvertY         = Save->bInvertY;
	bShowHUD         = Save->bShowHUD;
	CrosshairSize    = Save->CrosshairSize;
	CrosshairOpacity = Save->CrosshairOpacity;
	CrosshairColor   = Save->CrosshairColor;
}

void UComplyGameInstance::ApplyAudioSettings() const
{
	if (!MasterSoundMix) return;

	if (SFXSoundClass)
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, SFXSoundClass, SFXVolume, 1.f, 0.f);
	if (MusicSoundClass)
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, MusicSoundClass, MusicVolume, 1.f, 0.f);
	if (AmbienceSoundClass)
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, AmbienceSoundClass, AmbienceVolume, 1.f, 0.f);

	UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
}
