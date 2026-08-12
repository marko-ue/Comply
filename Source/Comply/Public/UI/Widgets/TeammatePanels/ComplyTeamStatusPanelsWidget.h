// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
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
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PanelsVerticalBox;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UComplyTeammatePanelWidget> TeammatePanelWidgetClass;
	
	UPROPERTY()
	TArray<TObjectPtr<UComplyTeammatePanelWidget>> TeammateWidgets;
};
