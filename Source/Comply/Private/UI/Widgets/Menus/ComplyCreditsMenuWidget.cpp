// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Menus/ComplyCreditsMenuWidget.h"

#include "Components/Button.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ComplyCreditsMenuWidget)

void UComplyCreditsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Close->OnClicked.AddDynamic(this, &UComplyCreditsMenuWidget::OnCloseClicked);
}
