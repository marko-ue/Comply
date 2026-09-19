// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Menus/ComplyMenuWidgetBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ComplyMenuWidgetBase)

void UComplyMenuWidgetBase::OnCloseClicked()
{
	OnClosed.Broadcast();
	RemoveFromParent();
}
