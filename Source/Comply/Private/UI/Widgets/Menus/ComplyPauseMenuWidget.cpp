// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/Menus/ComplyPauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ComplyPlayerController.h"
#include "UI/Widgets/Menus/ComplySettingsMenuWidget.h"

void UComplyPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Resume->OnClicked.AddDynamic(this, &UComplyPauseMenuWidget::OnResumeClicked);
	Button_Settings->OnClicked.AddDynamic(this, &UComplyPauseMenuWidget::OnSettingsClicked);
	Button_LeaveSession->OnClicked.AddDynamic(this, &UComplyPauseMenuWidget::OnLeaveSessionClicked);
	Button_QuitGame->OnClicked.AddDynamic(this, &UComplyPauseMenuWidget::OnQuitGameClicked);
	Button_Credits->OnClicked.AddDynamic(this, &UComplyPauseMenuWidget::OnCreditsClicked);
}

void UComplyPauseMenuWidget::OnResumeClicked()
{
	CloseWidget();
}

void UComplyPauseMenuWidget::OnSettingsClicked()
{
	if (!SettingsMenuClass) return;
	
	OpenSubMenu(SettingsMenuClass);
}

void UComplyPauseMenuWidget::OnCreditsClicked()
{
	
}

// Handles adding the specific sub menu to the viewport and hiding the pause menu as long as it's opened
void UComplyPauseMenuWidget::OpenSubMenu(const TSubclassOf<UComplyMenuWidgetBase> SubMenuClass)
{
	if (!SubMenuClass) return;

	UComplyMenuWidgetBase* SubMenu = CreateWidget<UComplyMenuWidgetBase>(GetOwningPlayer(), SubMenuClass);
	if (!SubMenu) return;

	SubMenu->OnClosed.AddDynamic(this, &UComplyPauseMenuWidget::OnSubMenuClosed);
	SubMenu->AddToViewport(11);
	SetVisibility(ESlateVisibility::Hidden);
}

// When any sub-menus are closed (like the settings and credits), the pause menu becomes visible again
void UComplyPauseMenuWidget::OnSubMenuClosed()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UComplyPauseMenuWidget::OnLeaveSessionClicked()
{
	CloseWidget();
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Lobby"));
}

void UComplyPauseMenuWidget::OnQuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UComplyPauseMenuWidget::CloseWidget() const
{
	AComplyPlayerController* PC = GetOwningPlayer<AComplyPlayerController>();
	if (!PC) return;

	PC->CloseMenuWidget();
}