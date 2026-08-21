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
    CachedCharacter = InCharacter;

    HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UComplyAttributeSet::GetHealthAttribute()
    ).AddUObject(this, &UComplyTeammatePanelWidget::OnHealthChanged);

    MaxHealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        UComplyAttributeSet::GetMaxHealthAttribute()
    ).AddUObject(this, &UComplyTeammatePanelWidget::OnMaxHealthChanged);

    bool bFound = false;
    CurrentHealth = ASC->GetGameplayAttributeValue(UComplyAttributeSet::GetHealthAttribute(), bFound);
    CurrentMaxHealth = ASC->GetGameplayAttributeValue(UComplyAttributeSet::GetMaxHealthAttribute(), bFound);

    if (CurrentMaxHealth > 0.f)
    {
        UpdateHealthBar();
    }

    if (TeammatePanelImage && InCharacter)
    {
        if (const TObjectPtr<UTexture2D>* FoundTexture = ClassPanelMap.Find(InCharacter->GetClass()))
        {
            TeammatePanelImage->SetBrushFromTexture(FoundTexture->Get());
        }
    }

    // Try weapon init immediately, start retry timer if not ready yet
    if (!TryInitializeWeapon())
    {
        GetWorld()->GetTimerManager().SetTimer(
            WeaponInitRetryHandle, this, &UComplyTeammatePanelWidget::RetryWeaponInit, 0.5f, true
        );
    }
}

bool UComplyTeammatePanelWidget::TryInitializeWeapon()
{
    if (!CachedCharacter || !ASC.IsValid()) return false;

    TSubclassOf<URangedWeaponAbilityBase> WeaponClass = CachedCharacter->EquippedPrimaryWeaponClass;
    if (!WeaponClass) return false;

    const URangedWeaponAbilityBase* WeaponCDO = WeaponClass->GetDefaultObject<URangedWeaponAbilityBase>();
    if (!WeaponCDO || !WeaponCDO->WeaponData) return false;

    if (LowAmmoImage)
    {
        LowAmmoImage->SetBrushFromTexture(WeaponCDO->WeaponData->BulletIcon);
        LowAmmoImage->SetRenderOpacity(0.f);
    }

    bool bMaxReserveFound = false;
    CachedMaxReserveAmmo = ASC->GetGameplayAttributeValue(
        WeaponCDO->GetMaxReserveAmmoAttribute(), bMaxReserveFound
    );
    CachedMaxReserveAmmo = FMath::Max(CachedMaxReserveAmmo, 1.f);

    if (ReserveAmmoChangedHandle.IsValid())
    {
        ASC->GetGameplayAttributeValueChangeDelegate(
            WeaponCDO->GetCurrentReserveAmmoAttribute()
        ).Remove(ReserveAmmoChangedHandle);
    }

    ReserveAmmoChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        WeaponCDO->GetCurrentReserveAmmoAttribute()
    ).AddUObject(this, &UComplyTeammatePanelWidget::OnReserveAmmoChanged);

    bool bReserveFound = false;
    const float InitialReserve = ASC->GetGameplayAttributeValue(
        WeaponCDO->GetCurrentReserveAmmoAttribute(), bReserveFound
    );
    UpdateLowAmmoIndicator(InitialReserve);

    return true;
}

void UComplyTeammatePanelWidget::RetryWeaponInit()
{
    UE_LOG(LogTemp, Warning, TEXT("[RetryWeaponInit] Firing"));
    if (TryInitializeWeapon())
    {
        GetWorld()->GetTimerManager().ClearTimer(WeaponInitRetryHandle);
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
