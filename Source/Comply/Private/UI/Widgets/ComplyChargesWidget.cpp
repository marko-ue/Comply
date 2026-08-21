// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyChargesWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/Data/Player/Grenades/ComplyGrenadeData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UComplyChargeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TryInitializeCharge();
}

void UComplyChargeWidget::TryInitializeCharge()
{
	const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetOwningPlayerPawn());
	if (!Character)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ChargeInitRetryHandle, this, &UComplyChargeWidget::TryInitializeCharge, 0.25f, false
		);
		return;
	}

	ActiveThrowable = Character->GetEquippedThrowable();
	if (!ActiveThrowable || !ActiveThrowable->GrenadeData)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ChargeInitRetryHandle, this, &UComplyChargeWidget::TryInitializeCharge, 0.25f, false
		);
		return;
	}

	// Past this point everything is available, run original init logic
	InitializeCharge();
}

void UComplyChargeWidget::InitializeCharge()
{
	const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetOwningPlayerPawn());
	if (!Character) return;

	ActiveThrowable = Character->GetEquippedThrowable();
	if (!ActiveThrowable || !ActiveThrowable->GrenadeData) return;

	ChargeImage->SetBrushFromTexture(ActiveThrowable->GrenadeData->ChargesIcon);

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
	if (!ASC) return;
	
	ChargeAttribute = ActiveThrowable->GetCurrentChargesAttribute();
	if (!ChargeAttribute.IsValid()) return;
	
	ChargeChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(ChargeAttribute)
		.AddUObject(this, &UComplyChargeWidget::OnChargeChanged);

	bool bFound = false;
	CachedCharge = ASC->GetGameplayAttributeValue(ChargeAttribute, bFound);

	bool bMaxFound = false;
	CachedMaxCharge = ASC->GetGameplayAttributeValue(ActiveThrowable->GetMaxChargesAttribute(), bMaxFound);

	RefreshChargeText();
}

void UComplyChargeWidget::OnChargeChanged(const FOnAttributeChangeData& Data)
{
	CachedCharge = Data.NewValue;
	RefreshChargeText();
}
 
void UComplyChargeWidget::RefreshChargeText() const
{
	if (ChargeText)
	{
		const FString ChargeString = FString::Printf(TEXT("%d / %d"),
			static_cast<int32>(CachedCharge),
			static_cast<int32>(CachedMaxCharge));
		ChargeText->SetText(FText::FromString(ChargeString));
	}
}

void UComplyChargeWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(ChargeInitRetryHandle);
}