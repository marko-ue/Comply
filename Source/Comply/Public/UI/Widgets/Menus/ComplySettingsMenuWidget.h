// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyMenuWidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "ComplySettingsMenuWidget.generated.h"


class UWidgetSwitcher;
class USlider;
class UCheckBox;
class UButton;
class USoundMix;
class USoundClass;

/**
 * 
 */
UCLASS()
class COMPLY_API UComplySettingsMenuWidget : public UComplyMenuWidgetBase
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_SFXVolume;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_MusicVolume;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_AmbienceVolume;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_Sensitivity;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCheckBox> CheckBox_InvertY;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCheckBox> CheckBox_ShowHUD;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_CrosshairSize;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_CrosshairOpacity;

    // RGB sliders for crosshair color
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_CrosshairR;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_CrosshairG;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> Slider_CrosshairB;

	// Buttons
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Apply;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Button_Close;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabAudio;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabControls;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabSession;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TabHUD;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_VoteKick;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> VoteKickWidgetClass;
	
	UFUNCTION()
	void OnTabAudioClicked();

	UFUNCTION()
	void OnTabControlsClicked();

	UFUNCTION()
	void OnTabSessionClicked();

	UFUNCTION()
	void OnTabHUDClicked();
	
	UFUNCTION()
	void OnVoteKickClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher_Settings;

    // Audio
    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundMix> MasterSoundMix;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> SFXSoundClass;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundClass> MusicSoundClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundClass> AmbienceSoundClass;
	
    UFUNCTION()
    void OnSFXVolumeChanged(float Value);

    UFUNCTION()
    void OnMusicVolumeChanged(float Value);
	
	UFUNCTION()
	void OnAmbienceVolumeChanged(float Value);

    UFUNCTION()
    void OnApplyClicked();

    // Helpers for applying modified settings
    void LoadCurrentSettings() const;
    void ApplyVolume(USoundClass* SoundClass, const float Volume) const;
    void ApplyCrosshairSettings() const;
    void ApplyHUDVisibility(bool bVisible) const;
};
