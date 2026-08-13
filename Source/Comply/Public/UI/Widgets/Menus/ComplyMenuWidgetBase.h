// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyMenuWidgetBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsMenuClosed);

/**
 * 
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
