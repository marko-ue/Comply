// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Menus/ComplySettingsMenuWidget.h"
#include "Components/Slider.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "UI/Widgets/ComplyCrosshairWidget.h"
#include "UI/Widgets/ComplyHUDWidget.h"
#include "ComplyPlayerController.h"
#include "Components/ComboBoxString.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "GameFramework/PlayerState.h"

void UComplySettingsMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Wire slider OnValueChanged — these fire every frame while dragging,
    // so we only apply volume live (cheap) and defer everything else to Apply.
    Slider_SFXVolume->OnValueChanged.AddDynamic(this, &UComplySettingsMenuWidget::OnSFXVolumeChanged);
    Slider_MusicVolume->OnValueChanged.AddDynamic(this, &UComplySettingsMenuWidget::OnMusicVolumeChanged);
    Slider_AmbienceVolume->OnValueChanged.AddDynamic(this, &UComplySettingsMenuWidget::OnAmbienceVolumeChanged);
    
    // Tab buttons
    Button_TabAudio->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnTabAudioClicked);
    Button_TabControls->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnTabControlsClicked);
    Button_TabSession->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnTabSessionClicked);
    Button_TabHUD->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnTabHUDClicked);
    
    Button_VoteKick->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnVoteKickClicked);

    // Start on Audio tab
    WidgetSwitcher_Settings->SetActiveWidgetIndex(0);

    Button_Apply->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnApplyClicked);
    Button_Close->OnClicked.AddDynamic(this, &UComplySettingsMenuWidget::OnCloseClicked);

    LoadCurrentSettings();
    
    PopulateVoteKickDropdown();
    
    AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
    if (GS)
    {
        GS->OnVoteKickResolved.AddDynamic(this, &UComplySettingsMenuWidget::OnVoteKickResolved);
    }
}

// Loads any settings already set into the widgets to update them
void UComplySettingsMenuWidget::LoadCurrentSettings() const
{
    const UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>();
    if (!GI) return;

    Slider_SFXVolume->SetValue(GI->SFXVolume);
    Slider_MusicVolume->SetValue(GI->MusicVolume);
    Slider_AmbienceVolume->SetValue(GI->AmbienceVolume);
    Slider_Sensitivity->SetValue(GI->LookSensitivity);
    CheckBox_InvertY->SetIsChecked(GI->bInvertY);
    CheckBox_ShowHUD->SetIsChecked(GI->bShowHUD);

    Slider_CrosshairSize->SetValue(GI->CrosshairSize);
    Slider_CrosshairOpacity->SetValue(GI->CrosshairOpacity);
    Slider_CrosshairR->SetValue(GI->CrosshairColor.R);
    Slider_CrosshairG->SetValue(GI->CrosshairColor.G);
    Slider_CrosshairB->SetValue(GI->CrosshairColor.B);
}

// Applies the new volume to the sound class whenever it's changed
void UComplySettingsMenuWidget::OnSFXVolumeChanged(float Value)
{
    ApplyVolume(SFXSoundClass, Value);
}

void UComplySettingsMenuWidget::OnMusicVolumeChanged(float Value)
{
    ApplyVolume(MusicSoundClass, Value);
}

void UComplySettingsMenuWidget::OnAmbienceVolumeChanged(float Value)
{
    ApplyVolume(AmbienceSoundClass, Value);
}

void UComplySettingsMenuWidget::ApplyVolume(USoundClass* SoundClass, const float Volume) const
{
    if (!MasterSoundMix || !SoundClass) return;
    
    UGameplayStatics::SetSoundMixClassOverride(
        GetWorld(),
        MasterSoundMix,
        SoundClass,
        Volume,
        1.f,
        0.f
    );
    UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
}

// When the apply button is clicked, all settings get set
void UComplySettingsMenuWidget::OnApplyClicked()
{
    UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>();
    if (!GI) return;

    // Persist values
    GI->SFXVolume       = Slider_SFXVolume->GetValue();
    GI->MusicVolume     = Slider_MusicVolume->GetValue();
    GI->AmbienceVolume  = Slider_AmbienceVolume->GetValue();
    GI->LookSensitivity = Slider_Sensitivity->GetValue();
    GI->bInvertY        = CheckBox_InvertY->IsChecked();
    GI->bShowHUD        = CheckBox_ShowHUD->IsChecked();

    GI->CrosshairSize    = Slider_CrosshairSize->GetValue();
    GI->CrosshairOpacity = Slider_CrosshairOpacity->GetValue();
    GI->CrosshairColor   = FLinearColor(
        Slider_CrosshairR->GetValue(),
        Slider_CrosshairG->GetValue(),
        Slider_CrosshairB->GetValue(),
        1.f
    );

    GI->SaveSettings();
    
    ApplyCrosshairSettings();
    ApplyHUDVisibility(GI->bShowHUD);
}

void UComplySettingsMenuWidget::ApplyCrosshairSettings() const
{
    const UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>();
    const AComplyPlayerController* PC = GetOwningPlayer<AComplyPlayerController>();
    if (!PC || !GI) return;
    
    UComplyCrosshairWidget* CrosshairWidget = PC->HUDWidget->GetCrosshairWidget();
    if (!CrosshairWidget) return;

    CrosshairWidget->SetCrosshairSize(GI->CrosshairSize);
    CrosshairWidget->SetCrosshairOpacity(GI->CrosshairOpacity);
    CrosshairWidget->SetCrosshairColor(GI->CrosshairColor);
}

void UComplySettingsMenuWidget::ApplyHUDVisibility(bool bVisible) const
{
    const AComplyPlayerController* PC = GetOwningPlayer<AComplyPlayerController>();
    if (!PC) return;
    
    UComplyHUDWidget* HUD = PC->HUDWidget;
    if (!HUD) return;

    HUD->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UComplySettingsMenuWidget::PopulateVoteKickDropdown()
{
    AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
    const APlayerController* LocalPC = GetOwningPlayer();
    if (!GS || !LocalPC) return;

    ComboBox_VoteKickTarget->ClearOptions();
    KickablePlayerStates.Empty();

    for (APlayerState* PS : GS->PlayerArray)
    {
        if (PS == LocalPC->PlayerState) continue;

        ComboBox_VoteKickTarget->AddOption(PS->GetPlayerName());
        KickablePlayerStates.Add(PS);
    }
}

// Re-evaluate what should be in the combo box when a vote kick resolves if the target was kicked
void UComplySettingsMenuWidget::OnVoteKickResolved(bool bKicked, APlayerState* Target)
{
    if (!bKicked) return;
    
    FTimerHandle RepopulateHandle;
    GetWorld()->GetTimerManager().SetTimer(RepopulateHandle, this,
        &UComplySettingsMenuWidget::PopulateVoteKickDropdown, 0.5f, false);
    
    PopulateVoteKickDropdown();
}

void UComplySettingsMenuWidget::OnTabAudioClicked()
{
    WidgetSwitcher_Settings->SetActiveWidgetIndex(0);
}

void UComplySettingsMenuWidget::OnTabControlsClicked()
{
    WidgetSwitcher_Settings->SetActiveWidgetIndex(1);
}

void UComplySettingsMenuWidget::OnTabHUDClicked()
{
    WidgetSwitcher_Settings->SetActiveWidgetIndex(2);
}

void UComplySettingsMenuWidget::OnTabSessionClicked()
{
    WidgetSwitcher_Settings->SetActiveWidgetIndex(3);
    PopulateVoteKickDropdown();
}

void UComplySettingsMenuWidget::OnVoteKickClicked()
{
    if (!VoteKickWidgetClass) return;

    AComplyPlayerController* PC = Cast<AComplyPlayerController>(GetOwningPlayer());
    if (!PC) return;

    const int32 SelectedIndex = ComboBox_VoteKickTarget->GetSelectedIndex();
    if (!KickablePlayerStates.IsValidIndex(SelectedIndex)) return;

    APlayerState* Target = KickablePlayerStates[SelectedIndex];
    PC->Server_InitiateVoteKick(Target);
    
    PopulateVoteKickDropdown();
}
