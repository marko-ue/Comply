// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Menus/ComplyMenuWidgetBase.h"

void UComplyMenuWidgetBase::OnCloseClicked()
{
	OnClosed.Broadcast();
	RemoveFromParent();
}
