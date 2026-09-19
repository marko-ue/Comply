// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "ComplyMenuWidgetBase.h"

#include "ComplyCreditsMenuWidget.generated.h"

class UButton;

/**
 * Credits menu widget containing text and a button to close it.
 */
UCLASS()
class COMPLY_API UComplyCreditsMenuWidget : public UComplyMenuWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Close;
};
