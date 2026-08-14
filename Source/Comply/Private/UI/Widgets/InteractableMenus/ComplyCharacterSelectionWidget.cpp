// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/InteractableMenus/ComplyCharacterSelectionWidget.h"

#include "ComplyPlayerController.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/Button.h"
#include "Framework/GameMode/ComplyGameModeBase.h"


namespace ComplyCharacterClasses
{
	// Resolved once in NativeConstruct via StaticLoadClass. Stored as statics so they are only looked up once
	static TSubclassOf<AComplyPlayerCharacter> Ranger    = nullptr;
	static TSubclassOf<AComplyPlayerCharacter> Enforcer  = nullptr;
	static TSubclassOf<AComplyPlayerCharacter> Disruptor = nullptr;
}

void UComplyCharacterSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SelectRangerButton->OnClicked.AddDynamic(this, &UComplyCharacterSelectionWidget::HandleRangerPressed);
	SelectEnforcerButton->OnClicked.AddDynamic(this, &UComplyCharacterSelectionWidget::HandleEnforcerPressed);
	SelectDisruptorButton->OnClicked.AddDynamic(this, &UComplyCharacterSelectionWidget::HandleDisruptorPressed);
	CloseButton->OnClicked.AddDynamic(this, &UComplyCharacterSelectionWidget::HandleCloseClicked);
	
	if (const APawn* OwningPawn = GetOwningPlayerPawn())
	{
		ComplyPlayerController = Cast<AComplyPlayerController>(OwningPawn->GetController());
	}
	
	if (const UWorld* World = GetWorld())
	{
		ComplyGameMode = Cast<AComplyGameModeBase>(World->GetAuthGameMode());
	}
	
	if (!ComplyCharacterClasses::Ranger)
	{
		ComplyCharacterClasses::Ranger = StaticLoadClass(
			AComplyPlayerCharacter::StaticClass(), nullptr,
			TEXT("/Game/Blueprints/Character/PlayerCharacter/Ranger/BP_Player_Ranger.BP_Player_Ranger_C")
		);
	}
	if (!ComplyCharacterClasses::Enforcer)
	{
		ComplyCharacterClasses::Enforcer = StaticLoadClass(
			AComplyPlayerCharacter::StaticClass(), nullptr,
			TEXT("/Game/Blueprints/Character/PlayerCharacter/Enforcer/BP_Player_Enforcer.BP_Player_Enforcer_C")
		);
	}
	if (!ComplyCharacterClasses::Disruptor)
	{
		ComplyCharacterClasses::Disruptor = StaticLoadClass(
			AComplyPlayerCharacter::StaticClass(), nullptr,
			TEXT("/Game/Blueprints/Character/PlayerCharacter/Disruptor/BP_Player_Disruptor.BP_Player_Disruptor_C")
		);
	}
}
	
void UComplyCharacterSelectionWidget::HandleRangerPressed()
{
	ConfirmSelection(ComplyCharacterClasses::Ranger);
}

void UComplyCharacterSelectionWidget::HandleEnforcerPressed()
{
	ConfirmSelection(ComplyCharacterClasses::Enforcer);
}

void UComplyCharacterSelectionWidget::HandleDisruptorPressed()
{
	ConfirmSelection(ComplyCharacterClasses::Disruptor);
}
 
void UComplyCharacterSelectionWidget::HandleCloseClicked()
{
	if (ComplyPlayerController)
	{
		ComplyPlayerController->CloseMenuWidget();
	}
}

void UComplyCharacterSelectionWidget::ConfirmSelection(TSubclassOf<AComplyPlayerCharacter> SelectedClass)
{
	if (!ComplyPlayerController || !SelectedClass) return; 
	
	// Selects the chosen character by calling the server RPC directly
	ComplyPlayerController->Server_SelectCharacter(SelectedClass);

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CloseDelayHandle, this, &UComplyCharacterSelectionWidget::OnCloseDelayFinished,
			SelectionCloseDelay, false
		);
	}
}

void UComplyCharacterSelectionWidget::OnCloseDelayFinished()
{
	if (ComplyPlayerController)
	{
		ComplyPlayerController->CloseMenuWidget();
	}
}

