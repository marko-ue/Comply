// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyMenuWidgetBase.h"
#include "Blueprint/UserWidget.h"
#include "ComplyPauseMenuWidget.generated.h"

class UComplySettingsMenuWidget;
class UButton;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyPauseMenuWidget : public UComplyMenuWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Settings;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_LeaveSession;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_QuitGame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Credits;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UComplySettingsMenuWidget> SettingsMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UComplySettingsMenuWidget> CreditsMenuClass;

	UFUNCTION()
	void OnResumeClicked();
	
	UFUNCTION()
	void OnSettingsClicked();
	
	void OpenSubMenu(const TSubclassOf<UComplyMenuWidgetBase> SubMenuClass);
	
	UFUNCTION()
	void OnSubMenuClosed();

	UFUNCTION()
	void OnLeaveSessionClicked();

	UFUNCTION()
	void OnQuitGameClicked();

	UFUNCTION()
	void OnCreditsClicked();
	
	void CloseWidget() const;
};
