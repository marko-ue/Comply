// Copyright Epic Games, Inc. All Rights Reserved.


#include "ComplyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Comply.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "Framework/GameMode/ComplyGameModeBase.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AComplyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
		} 
		else
		{

			UE_LOG(LogComply, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
	
	if (AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>())
	{
		SelectedCharacterClass = PS->LastSelectedCharacterClass ? PS->LastSelectedCharacterClass : DefaultCharacterClass;
	}
}

void AComplyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>())
	{
		SelectedCharacterClass = PS->LastSelectedCharacterClass ? PS->LastSelectedCharacterClass : DefaultCharacterClass;
	}
}

void AComplyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

// Ensures mapping contexts are added on the client whenever a pawn is possessed
void AComplyPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}

		if (!SVirtualJoystick::ShouldDisplayTouchInterface())
		{
			for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
}

void AComplyPlayerController::ShowFlashbangEffect()
{
	if (!FlashbangWidgetClass) return;
    
	// This widget displays a white image which fades out over 3 seconds, at which point the widget gets removed
	FlashbangWidget = CreateWidget<UUserWidget>(this, FlashbangWidgetClass);
	if (FlashbangWidget) FlashbangWidget->AddToViewport();
}

void AComplyPlayerController::OpenMenuWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (ActiveMenuWidget)
		ActiveMenuWidget->RemoveFromParent();

	ActiveMenuWidget = CreateWidget<UUserWidget>(this, WidgetClass);
	ActiveMenuWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(nullptr);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AComplyPlayerController::CloseMenuWidget()
{
	if (ActiveMenuWidget)
	{
		ActiveMenuWidget->RemoveFromParent();
		ActiveMenuWidget = nullptr;
	}
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void AComplyPlayerController::Server_SelectCharacter_Implementation(TSubclassOf<AComplyPlayerCharacter> SelectedCharacter)
{
	APawn* PreviousCharacterPawn = GetPawn();
	
	// Clear abilities before unpossessing so the player can activate abilities from the newly possessed character class
	if (AComplyCharacterBase* OldCharacter = Cast<AComplyCharacterBase>(PreviousCharacterPawn))
	{
		OldCharacter->ClearStartupAbilities();
	}
	
	UnPossess();
	
	if (PreviousCharacterPawn) { PreviousCharacterPawn->Destroy(); }
	
	if (SelectedCharacter)
	{
		SelectedCharacterClass = SelectedCharacter;
		
		if (AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>())
		{
			PS->LastSelectedCharacterClass = SelectedCharacter;
			if (UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>())
			{
				// The selected character class is added to the game instance map containing the player ID associated with that character
				GI->PlayerCharacterSelections.Add(FString::FromInt(PS->GetPlayerId()), SelectedCharacter);
			}
		}
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		AComplyPlayerCharacter* NewCharacter = GetWorld()->SpawnActor<AComplyPlayerCharacter>(
			SelectedCharacter, PreviousCharacterPawn->GetActorTransform(), SpawnParams);
		
		if (NewCharacter)
		{
			Possess(NewCharacter);
		}
		
		if (AComplyGameModeBase* GM = GetWorld()->GetAuthGameMode<AComplyGameModeBase>())
		{
			if (AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>())
			{
				// Updates the game state's variable whenever characters are changed
				GS->bAllPlayersHaveUniqueClasses = GM->AllPlayersHaveUniqueCharacters();
			}
			// Broadcast the player selection delegate. The mission selection widget listens to this and updates itself for feedback
			GM->OnPlayerSelectionChanged.Broadcast();
		}
	}
}
