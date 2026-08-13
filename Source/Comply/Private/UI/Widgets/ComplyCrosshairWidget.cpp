// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyCrosshairWidget.h"

#include "Components/Image.h"
#include "Framework/GameInstance/ComplyGameInstance.h"


void UComplyCrosshairWidget::InitializeCrosshair()
{
	// Crosshair initialized with settings previously set by the player
	if (const UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>())
	{
		SetRenderScale(FVector2D(GI->CrosshairSize / 32.f));
		SetCrosshairColor(GI->CrosshairColor);
		SetRenderOpacity(GI->CrosshairOpacity);
	}
}

void UComplyCrosshairWidget::SetCrosshairTexture(UTexture2D* CrosshairTexture)
{
	CrosshairImage->SetBrushFromTexture(CrosshairTexture);
}

void UComplyCrosshairWidget::SetCrosshairSize(float Size)
{
	SetRenderScale(FVector2D(Size / 32.f, Size / 32.f));
}

void UComplyCrosshairWidget::SetCrosshairColor(FLinearColor Color)
{
	if (CrosshairImage)
	{
		CrosshairImage->SetColorAndOpacity(Color);
	}
}

void UComplyCrosshairWidget::SetCrosshairOpacity(float Opacity)
{
	SetRenderOpacity(Opacity);
}
