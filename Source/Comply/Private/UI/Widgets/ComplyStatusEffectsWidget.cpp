// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyStatusEffectsWidget.h"

#include "AbilitySystemComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UComplyStatusEffectsWidget::InitializeStatusEffects(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;
 
	ASC = InASC;
	RegisterTagEvents();
}

void UComplyStatusEffectsWidget::NativeDestruct()
{
	UnregisterTagEvents();
	Super::NativeDestruct();
}

void UComplyStatusEffectsWidget::RegisterTagEvents()
{
	if (!ASC) return;
 
	// Registers a listener for every tag that has an icon entry in the map
	for (const TPair<FGameplayTag, TObjectPtr<UTexture2D>>& Entry : StatusEffectIcons)
	{
		FDelegateHandle Handle = ASC->RegisterGameplayTagEvent(Entry.Key, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &UComplyStatusEffectsWidget::OnTagChanged);
 
		TagDelegateHandles.Add(Handle);
	}
}

// Unregisters all tag events if the widget gets destroyed
void UComplyStatusEffectsWidget::UnregisterTagEvents()
{
	if (!ASC) return;
	
	int32 HandleIndex = 0;
	for (const TPair<FGameplayTag, TObjectPtr<UTexture2D>>& Entry : StatusEffectIcons)
	{
		if (TagDelegateHandles.IsValidIndex(HandleIndex))
		{
			ASC->RegisterGameplayTagEvent(Entry.Key, EGameplayTagEventType::NewOrRemoved)
				.Remove(TagDelegateHandles[HandleIndex]);
		}
		++HandleIndex;
	}
 
	TagDelegateHandles.Empty();
}

void UComplyStatusEffectsWidget::OnTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		AddIconForTag(Tag);
	}
	else
	{
		RemoveIconForTag(Tag);
	}
}

// Creates the image for the icon and adds it to the horizontal box
void UComplyStatusEffectsWidget::AddIconForTag(const FGameplayTag& Tag)
{
	// Guard against double-adding if the tag count goes 1->2 (for stacking GEs)
	if (ActiveIconWidgets.Contains(Tag)) return;
 
	const TObjectPtr<UTexture2D>* Texture = StatusEffectIcons.Find(Tag);
	if (!Texture || !(*Texture)) return;
 
	UImage* Icon = NewObject<UImage>(this);
 
	FSlateBrush Brush;
	Brush.SetResourceObject(*Texture);
	Brush.ImageSize = FVector2D(40.f, 40.f);
	Icon->SetBrush(Brush);
 
	UHorizontalBoxSlot* IconSlot = StatusIconsBox->AddChildToHorizontalBox(Icon);
	IconSlot->SetPadding(FMargin(4.f, 0.f, 4.f, 0.f));
	ActiveIconWidgets.Add(Tag, Icon);
 
	StatusIconsBox->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UComplyStatusEffectsWidget::RemoveIconForTag(const FGameplayTag& Tag)
{
	const TObjectPtr<UImage>* Icon = ActiveIconWidgets.Find(Tag);
	if (!Icon || !(*Icon)) return;
 
	(*Icon)->RemoveFromParent();
	ActiveIconWidgets.Remove(Tag);
 
	// If there are no more status effects, collapse the empty horizontal box
	if (StatusIconsBox->GetChildrenCount() == 0)
	{
		StatusIconsBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}
