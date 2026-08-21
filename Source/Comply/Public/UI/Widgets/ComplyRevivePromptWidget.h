// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyRevivePromptWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyRevivePromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RevivePromptText;
};
