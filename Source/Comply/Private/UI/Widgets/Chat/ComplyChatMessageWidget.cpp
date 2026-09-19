// Copyright © 2026 Marko. All rights reserved.

#include "UI/Widgets/Chat/ComplyChatMessageWidget.h"

// UE
#include "Components/TextBlock.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ComplyChatMessageWidget)

// Called by the chat box widget whenever sending a message, passing in the player's name and message
// It sets its text which will then be broadcasted to players
void UComplyChatMessageWidget::InitializeMessage(const FString& PlayerName, const FString& Message) const
{
	if (MessageText)
	{
		const FString Full = FString::Printf(TEXT("%s: %s"), *PlayerName, *Message);
		MessageText->SetText(FText::FromString(Full));
	}
}
