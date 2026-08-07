#include "UI/Widgets/ComplyAmmoWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "ComplyPlayerController.h"
#include "Components/TextBlock.h"
#include "Character/ComplyPlayerCharacter.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Components/Image.h"
#include "Framework/PlayerState/ComplyPlayerState.h"


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