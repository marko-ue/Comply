// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyCrosshairWidget.h"

#include "Components/Image.h"

void UComplyCrosshairWidget::InitializeCrosshair()
{
	
}

void UComplyCrosshairWidget::SetCrosshairTexture(UTexture2D* CrosshairTexture)
{
	CrosshairImage->SetBrushFromTexture(CrosshairTexture);
}
