// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "ComplyMenuWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsMenuClosed);

/**
 * Main menu. Contains all other menu widgets as elements and a close button.
 */
UCLASS()
class COMPLY_API UComplyMenuWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Event broadcasted whenever a menu widget closes. The pause menu uses it to become visible again once this sub-menu is closed
	UPROPERTY()
	FOnSettingsMenuClosed OnClosed;

protected:
	UFUNCTION()
	virtual void OnCloseClicked();
};
