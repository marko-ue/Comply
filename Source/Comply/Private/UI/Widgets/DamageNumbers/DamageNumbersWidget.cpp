// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/DamageNumbers/DamageNumbersWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Widgets/DamageNumbers/DamageNumberEntryWidget.h"

void UDamageNumbersWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Pre-populate pool with the widgets
	for (int32 i = 0; i < 15; i++)
	{
		UDamageNumberEntryWidget* Entry = CreateWidget<UDamageNumberEntryWidget>(GetOwningPlayer(), EntryWidgetClass);
		RootCanvas->AddChildToCanvas(Entry);
		Entry->SetVisibility(ESlateVisibility::Hidden);
		Entry->OwningPool = this;  // Back-ref so it can return itself
		Pool.Add(Entry);
	}
}

UDamageNumberEntryWidget* UDamageNumbersWidget::GetPooledEntry()
{
	for (UDamageNumberEntryWidget* Entry : Pool)
	{
		if (!Entry->bActive) return Entry;
	}
	// Pool exhausted, drop the damage number
	return nullptr;
}

void UDamageNumbersWidget::ShowDamageNumber(const float DamageAmount, const FVector& WorldPos, const FLinearColor Color)
{
	UDamageNumberEntryWidget* Entry = GetPooledEntry();
	if (!Entry) return;

	// Get the world location of where the hit happened to show the damage number there
	FVector2D ScreenPos;
	UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), WorldPos, ScreenPos);

	const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	ScreenPos /= DPIScale;

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Entry->Slot);
	if (!CanvasSlot) return;
	CanvasSlot->SetPosition(ScreenPos);

	Entry->DamageText->SetColorAndOpacity(Color);
	
	// Shows the damage number widget
	Entry->Activate(FMath::CeilToInt(DamageAmount), WorldPos);
}
