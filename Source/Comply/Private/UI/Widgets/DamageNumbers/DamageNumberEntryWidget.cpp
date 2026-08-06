// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/DamageNumbers/DamageNumberEntryWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UDamageNumberEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// ReturnToPool called when the widget animation finishes
	FWidgetAnimationDynamicEvent AnimEvent;
	AnimEvent.BindDynamic(this, &UDamageNumberEntryWidget::ReturnToPool);
	BindToAnimationFinished(FloatUpAnim, AnimEvent);
}

void UDamageNumberEntryWidget::Activate(const int32 DamageAmount, const FVector& InWorldPos)
{
	// Called when damage is received and calculated
	bActive = true;
	WorldPosition = InWorldPos;
	SetVisibility(ESlateVisibility::HitTestInvisible);
	DamageText->SetText(FText::AsNumber(DamageAmount));
	PlayAnimation(FloatUpAnim);
}

// Ensures the damage number stays at its original spot, prevents it from moving with the camera
void UDamageNumberEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bActive) return;

	FVector2D ScreenPos;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WorldPosition, ScreenPos);

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	ScreenPos /= DPIScale;
	
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CanvasSlot->SetPosition(ScreenPos);
	}
}

// Animation ended, so the damage number should not be visible anymore
void UDamageNumberEntryWidget::ReturnToPool()
{
	bActive = false;
	SetVisibility(ESlateVisibility::Hidden);
}
