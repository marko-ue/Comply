// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyUtilityCooldownsWidget.h"

#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/ComplyUtilityData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UComplyUtilityCooldownsWidget::InitializeUtilityCooldown()
{
	const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetOwningPlayerPawn());
	if (!Character) return;

	ActiveUtility = Character->GetEquippedUtility();
	if (!ActiveUtility || !ActiveUtility->UtilityData) return;

	UtilityImage->SetBrushFromTexture(ActiveUtility->UtilityData->UtilityIcon, true);
	CooldownText->SetVisibility(ESlateVisibility::Hidden); // Initially the ability will not be on cooldown
}

void UComplyUtilityCooldownsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshCooldown();
}

void UComplyUtilityCooldownsWidget::RefreshCooldown() const
{
	if (!ActiveUtility) return;

	// Get the equipped utility ability's cooldown and store it
	float TimeRemaining = 0.f;
	float Duration = 0.f;
	ActiveUtility->GetCooldownTimeRemainingAndDuration(
		ActiveUtility->GetCurrentAbilitySpecHandle(),
		ActiveUtility->GetCurrentActorInfo(),
		TimeRemaining,
		Duration
	);
	
	if (TimeRemaining > 0.f)
	{
		CooldownText->SetVisibility(ESlateVisibility::Visible);
		CooldownText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::CeilToInt(TimeRemaining))));
	}
	else
	{
		CooldownText->SetVisibility(ESlateVisibility::Hidden);
	}
}
