// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyAmmoWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/TextBlock.h"
#include "Character/ComplyPlayerCharacter.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Components/Image.h"


void UComplyAmmoWidget::NativeConstruct()
{
    Super::NativeConstruct();
    TryInitializeAmmo();
}

void UComplyAmmoWidget::TryInitializeAmmo()
{
    const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetOwningPlayerPawn());
    if (!Character)
    {
        GetWorld()->GetTimerManager().SetTimer(
            AmmoInitRetryHandle, this, &UComplyAmmoWidget::TryInitializeAmmo, 0.25f, false
        );
        return;
    }

    ActiveWeapon = Character->GetEquippedPrimaryWeapon();
    if (!ActiveWeapon || !ActiveWeapon->WeaponData)
    {
        GetWorld()->GetTimerManager().SetTimer(
            AmmoInitRetryHandle, this, &UComplyAmmoWidget::TryInitializeAmmo, 0.25f, false
        );
        return;
    }

    // Past this point everything is available, run original init logic
    InitializeAmmo();
}

void UComplyAmmoWidget::InitializeAmmo()
{
    const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetOwningPlayerPawn());
    if (!Character) return;

    ActiveWeapon = Character->GetEquippedPrimaryWeapon();
    if (!ActiveWeapon || !ActiveWeapon->WeaponData) return;
    
    BulletImage->SetBrushFromTexture(ActiveWeapon->WeaponData->BulletIcon);
    
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
    if (!ASC) return;

    // Mag ammo
    ActiveAmmoAttribute = ActiveWeapon->GetCurrentAmmoAttribute();
    AmmoChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(ActiveAmmoAttribute)
        .AddUObject(this, &UComplyAmmoWidget::OnAmmoChanged);

    // Reserve ammo
    ActiveReserveAmmoAttribute = ActiveWeapon->GetCurrentReserveAmmoAttribute();
    ReserveAmmoChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(ActiveReserveAmmoAttribute)
        .AddUObject(this, &UComplyAmmoWidget::OnReserveAmmoChanged);

    bool bFound = false;
    CachedAmmo = ASC->GetGameplayAttributeValue(ActiveAmmoAttribute, bFound);
    
    bool bReserveFound = false;
    CachedReserveAmmo = ASC->GetGameplayAttributeValue(ActiveReserveAmmoAttribute, bReserveFound);
    
    bool bMaxFound = false;
    CachedMaxAmmo = ASC->GetGameplayAttributeValue(ActiveWeapon->GetMaxAmmoAttribute(), bMaxFound);
    
    bool bMaxReserveFound = false;
    CachedMaxReserveAmmo = ASC->GetGameplayAttributeValue(ActiveWeapon->GetMaxReserveAmmoAttribute(), bMaxReserveFound);

    RefreshAmmoText();
    RefreshReserveAmmoText();
}

void UComplyAmmoWidget::OnAmmoChanged(const FOnAttributeChangeData& Data)
{
    CachedAmmo = Data.NewValue;
    RefreshAmmoText();
}

void UComplyAmmoWidget::OnReserveAmmoChanged(const FOnAttributeChangeData& Data)
{
    CachedReserveAmmo = Data.NewValue;
    RefreshReserveAmmoText();
}

void UComplyAmmoWidget::RefreshAmmoText() const
{
    if (AmmoText)
    {
        const FString AmmoString = FString::Printf(TEXT("%d / %d"), static_cast<int32>(CachedAmmo), static_cast<int32>(CachedMaxAmmo));
        AmmoText->SetText(FText::FromString(AmmoString));
    }
}

void UComplyAmmoWidget::RefreshReserveAmmoText() const
{
    if (ReserveAmmoText)
    {
        const FString ReserveString = FString::Printf(TEXT("%d / %d"), static_cast<int32>(CachedReserveAmmo), static_cast<int32>(CachedMaxReserveAmmo));
        ReserveAmmoText->SetText(FText::FromString(ReserveString));
    }
}

void UComplyAmmoWidget::NativeDestruct()
{
    Super::NativeDestruct();
    GetWorld()->GetTimerManager().ClearTimer(AmmoInitRetryHandle);
}
