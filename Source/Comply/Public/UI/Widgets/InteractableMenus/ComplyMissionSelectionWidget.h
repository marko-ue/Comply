// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "ComplyMissionSelectionWidget.generated.h"

class UCheckBox;
class AComplyPlayerController;
class AComplyGameStateBase;
class AComplyGameModeBase;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyMissionSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectDataExtractionButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> FriendlyFireCheckBox;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnPlayerSelectionChanged();

	UFUNCTION()
	void OnSelectDataExtractionPressed();

	UFUNCTION()
	void OnClosePressed();
	
	UFUNCTION()
	void OnFriendlyFireChanged(bool bIsChecked);

	void UpdateStartButtonText() const;

	AComplyGameModeBase* GetComplyGameMode() const;
	AComplyGameStateBase* GetComplyGameState() const;
	AComplyPlayerController* GetComplyPlayerController() const;
	UComplyGameInstance* GetComplyGameInstance() const;
};
