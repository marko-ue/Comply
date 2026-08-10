// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ComplyStatusEffectsWidget.generated.h"

class UImage;
class UAbilitySystemComponent;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyStatusEffectsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeStatusEffects(UAbilitySystemComponent* InASC);
 
protected:
	virtual void NativeDestruct() override;
 
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> StatusIconsBox;
	
	// Maps each status effect tag to its icon
	UPROPERTY(EditDefaultsOnly, Category = "Status Effects")
	TMap<FGameplayTag, TObjectPtr<UTexture2D>> StatusEffectIcons;
 
	// Tracks which tag currently has which UImage slot so it can be removed cleanly
	TMap<FGameplayTag, TObjectPtr<UImage>> ActiveIconWidgets;
 
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
 
	// Registered tag event delegates stored so they can be unregistered on destruct
	TArray<FDelegateHandle> TagDelegateHandles;
	
	void RegisterTagEvents();
	void UnregisterTagEvents();
 
	void OnTagChanged(const FGameplayTag Tag, int32 NewCount);
 
	void AddIconForTag(const FGameplayTag& Tag);
	void RemoveIconForTag(const FGameplayTag& Tag);
};
