// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyChatMessageWidget.generated.h"

class UTextBlock;
/**
 * 
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
