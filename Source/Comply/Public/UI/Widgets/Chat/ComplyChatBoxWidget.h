// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyChatBoxWidget.generated.h"

class UComplyChatMessageWidget;
class UEditableTextBox;
class UVerticalBox;
class UScrollBox;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyChatBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void AddMessage(const FString& PlayerName, const FString& Message);
	
	void OpenInput();
	void CloseInput() const;
	
protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnMessageTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> MessageScroll;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MessageListVerticalBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInput;

	UPROPERTY(EditDefaultsOnly, Category = "Chat")
	TSubclassOf<UComplyChatMessageWidget> MessageWidgetClass;

	static constexpr int32 MaxMessages = 50;
	
	// The widget gets hidden after 10 seconds if there are no new messages and the player doesn't open the input
	// The timer is reset if either happens
	FTimerHandle HideTimerHandle;
	static constexpr float HideDelay = 10.f;

	void ResetHideTimer();
	void OnHideTimerExpired();
};
