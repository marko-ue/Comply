// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyMenuWidgetBase.h"
#include "ComplyCreditsMenuWidget.generated.h"

class UButton;
/**
 * 
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
