// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "ComplyRevivePromptWidget.generated.h"

class UTextBlock;

/**
 * Widget that shows a prompt for a revive when a player is hovering over a downed player.
 */
UCLASS()
class COMPLY_API UComplyRevivePromptWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RevivePromptText;
};
