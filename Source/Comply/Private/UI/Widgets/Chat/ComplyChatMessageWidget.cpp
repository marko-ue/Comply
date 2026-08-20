// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Chat/ComplyChatMessageWidget.h"
#include "Components/TextBlock.h"


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
