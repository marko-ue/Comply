// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Chat/ComplyChatBoxWidget.h"
#include "UI/Widgets/Chat/ComplyChatMessageWidget.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/EditableTextBox.h"
#include "ComplyPlayerController.h"


void UComplyChatBoxWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ChatInput)
    {
        ChatInput->OnTextCommitted.AddDynamic(this, &UComplyChatBoxWidget::OnMessageTextCommitted);
        ChatInput->SetVisibility(ESlateVisibility::Collapsed);
    }
    
    SetVisibility(ESlateVisibility::Hidden);
}
 
// Adds the message broadcasted by the client RPC to the message list
void UComplyChatBoxWidget::AddMessage(const FString& PlayerName, const FString& Message)
{
    if (!MessageWidgetClass || !MessageListVerticalBox) return;
    
    ResetHideTimer();

    // Trim oldest message if over cap
    if (MessageListVerticalBox->GetChildrenCount() >= MaxMessages)
    {
        MessageListVerticalBox->RemoveChildAt(0);
    }

    UComplyChatMessageWidget* MsgWidget = CreateWidget<UComplyChatMessageWidget>(GetOwningPlayer(), MessageWidgetClass);
    if (!MsgWidget) return;

    // Pass in the name and message to the widget and add it to the chat box by adding the widget itself (which is just a text block)
    MsgWidget->InitializeMessage(PlayerName, Message);
    MessageListVerticalBox->AddChild(MsgWidget);

    if (MessageScroll)
    {
        MessageScroll->ScrollToEnd();
    }
}

// This function is called whenever the input for using chat is pressed. It shows the input widget and allows the user to put in text
void UComplyChatBoxWidget::OpenInput()
{
    if (!ChatInput) return;
    
    ResetHideTimer();
    
    ChatInput->SetVisibility(ESlateVisibility::Visible);
    ChatInput->SetKeyboardFocus();

    if (APlayerController* PC = GetOwningPlayer())
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(ChatInput->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
        PC->SetInputMode(InputMode);
    }
}

void UComplyChatBoxWidget::CloseInput() const
{
    if (ChatInput)
    {
        ChatInput->SetText(FText::GetEmpty());
        ChatInput->SetVisibility(ESlateVisibility::Collapsed);
    }

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->SetInputMode(FInputModeGameOnly());
    }
}

// When the message is sent, call the RPC that handles broadcasting the message
void UComplyChatBoxWidget::OnMessageTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        const FString TrimmedText = Text.ToString().TrimStartAndEnd();
        if (!TrimmedText.IsEmpty())
        {
            if (AComplyPlayerController* PC = Cast<AComplyPlayerController>(GetOwningPlayer()))
            {
                PC->Server_SendChatMessage(TrimmedText);
            }
        }
    }

    // Close on Enter or Escape
    if (CommitMethod == ETextCommit::OnEnter || CommitMethod == ETextCommit::OnCleared)
    {
        CloseInput();
    }
}

void UComplyChatBoxWidget::ResetHideTimer()
{
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    
    if (const UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            HideTimerHandle, this, &UComplyChatBoxWidget::OnHideTimerExpired, HideDelay, false
        );
    }
}

void UComplyChatBoxWidget::OnHideTimerExpired()
{
    SetVisibility(ESlateVisibility::Hidden);
}
