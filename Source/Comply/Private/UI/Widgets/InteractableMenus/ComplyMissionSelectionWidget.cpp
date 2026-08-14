// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/InteractableMenus/ComplyMissionSelectionWidget.h"

#include "ComplyPlayerController.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CheckBox.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "GameFramework/Pawn.h"
#include "Framework/GameMode/ComplyGameModeBase.h"
#include "Framework/GameState/ComplyGameStateBase.h"


void UComplyMissionSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SelectDataExtractionButton->OnPressed.AddDynamic(this, &UComplyMissionSelectionWidget::OnSelectDataExtractionPressed);
    CloseButton->OnPressed.AddDynamic(this, &UComplyMissionSelectionWidget::OnClosePressed);
    
    FriendlyFireCheckBox->OnCheckStateChanged.AddDynamic(this, &UComplyMissionSelectionWidget::OnFriendlyFireChanged);
    
    const AComplyPlayerController* PC = GetComplyPlayerController();
    const bool bIsServer = PC && PC->HasAuthority();

    // If on the server, bind to the delegate which fires whenever a character is selected
    // Otherwise, notify the client that only the server may start a mission
    if (bIsServer)
    {
        UpdateStartButtonText();

        AComplyGameStateBase* GS = GetComplyGameState();
        AComplyGameModeBase* GM = GetComplyGameMode();
        
        if (GS && GM)
        {
            FriendlyFireCheckBox->SetIsChecked(GS->bFriendlyFire); // Initialize the widget status with the variable
            GM->OnPlayerSelectionChanged.AddDynamic(this, &UComplyMissionSelectionWidget::OnPlayerSelectionChanged);
        }
    }
    else
    {
        StatusText->SetText(FText::FromString(TEXT("Only the session owner may start")));
    }
}

void UComplyMissionSelectionWidget::NativeDestruct()
{
    if (AComplyGameModeBase* GM = GetComplyGameMode())
    {
        GM->OnPlayerSelectionChanged.RemoveDynamic(this, &UComplyMissionSelectionWidget::OnPlayerSelectionChanged);
    }

    Super::NativeDestruct();
}

void UComplyMissionSelectionWidget::OnPlayerSelectionChanged()
{
    UpdateStartButtonText();
}

// If on the server and all player classes are unique, server travel to the map. Otherwise return
void UComplyMissionSelectionWidget::OnSelectDataExtractionPressed()
{
    AComplyPlayerController* PC = GetComplyPlayerController();
    if (!PC || !PC->HasAuthority()) return;

    const AComplyGameStateBase* GS = GetComplyGameState();
    if (!GS || !GS->bAllPlayersHaveUniqueClasses) return;

    UWidgetBlueprintLibrary::SetInputMode_GameOnly(PC);

    if (AComplyGameModeBase* GM = GetComplyGameMode())
    {
        GM->TravelToMap(TEXT("/Game/Maps/ExtractDataMap?listen"));
    }
}

void UComplyMissionSelectionWidget::OnClosePressed()
{
    if (AComplyPlayerController* PC = GetComplyPlayerController())
    {
        PC->CloseMenuWidget();
    }
}

void UComplyMissionSelectionWidget::OnFriendlyFireChanged(bool bIsChecked)
{
    if (UComplyGameInstance* GI = GetComplyGameInstance())
    {
        GI->bFriendlyFire = bIsChecked;
    }
}

// Updates the status text based on if all players have unique classes or not
void UComplyMissionSelectionWidget::UpdateStartButtonText() const
{
    const AComplyGameStateBase* GS = GetComplyGameState();
    if (!GS) return;

    const FText NewText = GS->bAllPlayersHaveUniqueClasses
        ? FText::FromString(TEXT("Start"))
        : FText::FromString(TEXT("All players must have unique classes"));

    StatusText->SetText(NewText);
}

AComplyGameModeBase* UComplyMissionSelectionWidget::GetComplyGameMode() const
{
    return Cast<AComplyGameModeBase>(UGameplayStatics::GetGameMode(this));
}

AComplyGameStateBase* UComplyMissionSelectionWidget::GetComplyGameState() const
{
    return Cast<AComplyGameStateBase>(UGameplayStatics::GetGameState(this));
}

AComplyPlayerController* UComplyMissionSelectionWidget::GetComplyPlayerController() const
{
    return Cast<AComplyPlayerController>(GetOwningPlayer());
}

UComplyGameInstance* UComplyMissionSelectionWidget::GetComplyGameInstance() const
{
    return GetGameInstance<UComplyGameInstance>();
}