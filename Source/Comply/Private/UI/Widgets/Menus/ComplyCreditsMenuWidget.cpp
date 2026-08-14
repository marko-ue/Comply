// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Menus/ComplyCreditsMenuWidget.h"

#include "Components/Button.h"

void UComplyCreditsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Close->OnClicked.AddDynamic(this, &UComplyCreditsMenuWidget::OnCloseClicked);
}
