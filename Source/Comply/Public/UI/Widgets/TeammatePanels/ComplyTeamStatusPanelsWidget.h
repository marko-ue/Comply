// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "ComplyTeamStatusPanelsWidget.generated.h"

class AComplyPlayerState;
class AComplyPlayerCharacter;
class UComplyTeammatePanelWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyTeamStatusPanelsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeTeamStatusPanels();
	
protected:
	virtual void NativeConstruct() override;
	void StartPanelRetryTimer();
	void RetryPendingPanels();
	void AddPanelForPlayer(AComplyPlayerState* ComplyPS, AComplyPlayerCharacter* Character,
	                       UAbilitySystemComponent* ASC);
	virtual void NativeDestruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PanelsVerticalBox;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UComplyTeammatePanelWidget> TeammatePanelWidgetClass;
	
	UPROPERTY()
	TArray<TObjectPtr<UComplyTeammatePanelWidget>> TeammateWidgets;
	
	UFUNCTION()
	void OnVoteKickResolved(bool bKicked, APlayerState* Target);
	
	FTimerHandle PanelRetryTimerHandle;
};
