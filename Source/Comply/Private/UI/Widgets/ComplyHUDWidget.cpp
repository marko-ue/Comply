// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyHUDWidget.h"

#include "UI/Widgets/ComplyAmmoWidget.h"
#include "UI/Widgets/ComplyHealthBarWidget.h"

void UComplyHUDWidget::InitializeHUD(UAbilitySystemComponent* ASC)
{
	if (ComplyHealthBarWidget)
	{
		ComplyHealthBarWidget->InitializeHealthBar(ASC);
	}
	
	if (ComplyAmmoWidget)
	{
		ComplyAmmoWidget->InitializeAmmo();
	}
}
