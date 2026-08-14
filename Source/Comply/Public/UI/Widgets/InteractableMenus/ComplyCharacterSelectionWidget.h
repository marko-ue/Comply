// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyCharacterSelectionWidget.generated.h"

class AComplyPlayerCharacter;
class AComplyGameModeBase;
class AComplyPlayerController;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyCharacterSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
 
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectRangerButton;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectEnforcerButton;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectDisruptorButton;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> RangerPortrait;
 
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> EnforcerPortrait;
 
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> DisruptorPortrait;

	virtual void NativeConstruct() override;
 
private:
	UPROPERTY()
	TObjectPtr<AComplyPlayerController> ComplyPlayerController;
 
	UPROPERTY()
	TObjectPtr<AComplyGameModeBase> ComplyGameMode;
	
	UFUNCTION()
	void HandleRangerPressed();
 
	UFUNCTION()
	void HandleEnforcerPressed();
 
	UFUNCTION()
	void HandleDisruptorPressed();
 
	UFUNCTION()
	void HandleCloseClicked();
	
	void ConfirmSelection(TSubclassOf<AComplyPlayerCharacter> SelectedClass);
	
	static constexpr float SelectionCloseDelay = 0.2f;
	
	FTimerHandle CloseDelayHandle;
	
	UFUNCTION()
	void OnCloseDelayFinished();
};
