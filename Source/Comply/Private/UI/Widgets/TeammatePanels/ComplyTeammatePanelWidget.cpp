// Copyright © 2026 Marko. All rights reserved.

#include "UI/Widgets/TeammatePanels/ComplyTeammatePanelWidget.h"

#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "Engine/Texture2D.h"
#include "Framework/GameInstance/ComplyGameInstance.h"


void UComplyTeammatePanelWidget::InitializePanel(UAbilitySystemComponent* InASC, AComplyPlayerCharacter* InCharacter, AComplyPlayerState* InPlayerState)
{
    ASC = InASC;
    PlayerState = InPlayerState;

    HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UComplyAttributeSet::GetHealthAttribute()
    ).AddUObject(this, &UComplyTeammatePanelWidget::OnHealthChanged);

    MaxHealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UComplyAttributeSet::GetMaxHealthAttribute()
    ).AddUObject(this, &UComplyTeammatePanelWidget::OnMaxHealthChanged);

    bool bFound = false;
    CurrentHealth = ASC->GetGameplayAttributeValue(UComplyAttributeSet::GetHealthAttribute(), bFound);
    CurrentMaxHealth = ASC->GetGameplayAttributeValue(UComplyAttributeSet::GetMaxHealthAttribute(), bFound);
    
    // Getting the owning player pawn to bind to ammo attribute change callbacks
    const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetOwningPlayerPawn());
    if (!Character) return;

    const URangedWeaponAbilityBase* ActiveWeapon = Character->GetEquippedPrimaryWeapon();
    if (!ActiveWeapon || !ActiveWeapon->WeaponData) return;
    
    if (LowAmmoImage)
    {
        LowAmmoImage->SetBrushFromTexture(ActiveWeapon->WeaponData->BulletIcon);
        LowAmmoImage->SetRenderOpacity(0.f);
    }

    if (ActiveWeapon && ActiveWeapon->WeaponData)
    {
        bool bMaxReserveFound = false;
        CachedMaxReserveAmmo = ASC->GetGameplayAttributeValue(
            ActiveWeapon->GetMaxReserveAmmoAttribute(), bMaxReserveFound
        );
        CachedMaxReserveAmmo = FMath::Max(CachedMaxReserveAmmo, 1.f);

        ReserveAmmoChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
            ActiveWeapon->GetCurrentReserveAmmoAttribute()
        ).AddUObject(this, &UComplyTeammatePanelWidget::OnReserveAmmoChanged);

        bool bReserveFound = false;
        const float InitialReserve = ASC->GetGameplayAttributeValue(
            ActiveWeapon->GetCurrentReserveAmmoAttribute(), bReserveFound
        );
        UpdateLowAmmoIndicator(InitialReserve);
    }

    UpdateHealthBar();
    
    // Sets the appropriate portraits based on who's initializing the widget
    if (InCharacter && TeammatePanelImage)
    {
        if (const TObjectPtr<UTexture2D>* FoundTexture = ClassPanelMap.Find(InCharacter->GetClass()))
        {
            TeammatePanelImage->SetBrushFromTexture(FoundTexture->Get());
        }
    }
}

void UComplyTeammatePanelWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    CurrentHealth = Data.NewValue;
    UpdateHealthBar();
}

void UComplyTeammatePanelWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
    CurrentMaxHealth = Data.NewValue;
    UpdateHealthBar();
}

void UComplyTeammatePanelWidget::OnReserveAmmoChanged(const FOnAttributeChangeData& Data)
{
    UpdateLowAmmoIndicator(Data.NewValue);
}

void UComplyTeammatePanelWidget::UpdateHealthBar()
{
    if (!HealthProgressBar) return;

    const float Percent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;
    HealthProgressBar->SetPercent(Percent);
}

// If the current reserve ammo is below the low ammo threshold, display the low ammo image and play its animation, otherwise hide it
void UComplyTeammatePanelWidget::UpdateLowAmmoIndicator(float NewReserveAmmo)
{
    if (!LowAmmoImage || !LowAmmoFlashAnimation) return;

    const float Ratio = NewReserveAmmo / CachedMaxReserveAmmo;
    const bool bIsLow = Ratio <= LowAmmoThreshold;

    if (bIsLow && !IsAnimationPlaying(LowAmmoFlashAnimation))
    {
        PlayAnimation(LowAmmoFlashAnimation, 0.f, 0);
    }
    else if (!bIsLow && IsAnimationPlaying(LowAmmoFlashAnimation))
    {
        StopAnimation(LowAmmoFlashAnimation);
        LowAmmoImage->SetRenderOpacity(0.f);
    }
}
