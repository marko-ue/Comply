// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "ComplyChatMessageWidget.generated.h"

class UTextBlock;

/**
 * Widget that contains the text inputted by the player to be displayed in the chat box.
 */
UCLASS()
class COMPLY_API UComplyChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeMessage(const FString& PlayerName, const FString& Message) const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;
};
