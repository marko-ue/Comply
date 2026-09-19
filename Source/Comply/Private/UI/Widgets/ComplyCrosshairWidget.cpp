// Copyright © 2026 Marko. All rights reserved.

#include "UI/Widgets/ComplyCrosshairWidget.h"

// Comply
#include "Framework/GameInstance/ComplyGameInstance.h"

// UE
#include "Components/Image.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ComplyCrosshairWidget)

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
