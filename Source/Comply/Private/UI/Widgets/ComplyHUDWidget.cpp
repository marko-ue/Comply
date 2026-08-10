// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyHUDWidget.h"

#include "AbilitySystemComponent.h"
#include "UI/Widgets/ComplyAmmoWidget.h"
#include "UI/Widgets/ComplyChargesWidget.h"
#include "UI/Widgets/ComplyHealthBarWidget.h"
#include "UI/Widgets/ComplyReviveProgressWidget.h"
#include "UI/Widgets/ComplyStatusEffectsWidget.h"
#include "UI/Widgets/ComplyUtilityCooldownsWidget.h"


void UComplyHUDWidget::InitializeHUD(UAbilitySystemComponent* ASC)
{
	CachedASC = ASC;
	TryInitializeWidgets();
}

void UComplyHUDWidget::TryInitializeWidgets()
{
	if (ComplyHealthBarWidget)
	{
		ComplyHealthBarWidget->InitializeHealthBar(CachedASC);
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
}

