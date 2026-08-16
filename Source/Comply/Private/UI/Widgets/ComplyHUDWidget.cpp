// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "UI/Widgets/ComplyAmmoWidget.h"
#include "UI/Widgets/ComplyChargesWidget.h"
#include "UI/Widgets/ComplyCrosshairWidget.h"
#include "UI/Widgets/ComplyHealthBarWidget.h"
#include "UI/Widgets/ComplyReviveProgressWidget.h"
#include "UI/Widgets/ComplyStatusEffectsWidget.h"
#include "UI/Widgets/ComplyUtilityCooldownsWidget.h"
#include "UI/Widgets/TeammatePanels/ComplyTeamStatusPanelsWidget.h"


void UComplyHUDWidget::InitializeHUD(UAbilitySystemComponent* ASC)
{
	// As soon as the HUD is initialized, the stored audio settings can now be applied here explicitly
	if (const UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>())
	{
		GI->ApplyAudioSettings();
	}
	
	CachedASC = ASC;
	TryInitializeWidgets();
}

void UComplyHUDWidget::TryInitializeWidgets()
{
	if (ComplyHealthBarWidget)
	{
		ComplyHealthBarWidget->InitializeHealthBar(CachedASC);
	}
	
	if (ComplyCrosshairWidget)
	{
		ComplyCrosshairWidget->InitializeCrosshair();
	}
	
	if (ComplyAmmoWidget)
	{
		ComplyAmmoWidget->InitializeAmmo();
	}
	
	if (ComplyChargeWidget)
	{
		ComplyChargeWidget->InitializeCharge();
	}
	
	if (ComplyUtilityCooldownsWidget)
	{
		ComplyUtilityCooldownsWidget->InitializeUtilityCooldown();
	}
	
	if (ComplyStatusEffectsWidget)
	{
		ComplyStatusEffectsWidget->InitializeStatusEffects(CachedASC);
	}
	
	if (ComplyReviveProgressWidget)
	{
		ComplyReviveProgressWidget->InitializeReviveProgress(CachedASC);
	}
	
	if (ComplyTeamStatusPanelsWidget)
	{
		ComplyTeamStatusPanelsWidget->InitializeTeamStatusPanels();
	}
}

