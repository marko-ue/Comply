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
#include "UI/Widgets/ComplyHUDWidget.h"
#include "UI/Widgets/ComplyVoteKickWidget.h"
#include "UI/Widgets/DamageNumbers/DamageNumbersWidget.h"
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
	
	if (const AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>())
	{
		SelectedCharacterClass = PS->LastSelectedCharacterClass ? PS->LastSelectedCharacterClass : DefaultCharacterClass;
	}
	
	if (!IsLocalController()) return;

	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS)
	{
		GS->OnVoteKickInitiated.AddDynamic(this, &AComplyPlayerController::OnVoteKickInitiated);
	}
	else
	{
		// GameState not ready yet, wait for it
		GetWorldTimerManager().SetTimer(GameStateWaitTimerHandle, this, &AComplyPlayerController::TryBindGameStateEvents, 0.1f, true);
	}
}

void AComplyPlayerController::TryBindGameStateEvents()
{
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS)
	{
		GS->OnVoteKickInitiated.AddDynamic(this, &AComplyPlayerController::OnVoteKickInitiated);
		GetWorldTimerManager().ClearTimer(GameStateWaitTimerHandle);
	}
}

void AComplyPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (const AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>())
	{
		SelectedCharacterClass = PS->LastSelectedCharacterClass ? PS->LastSelectedCharacterClass : DefaultCharacterClass;
	}
	
	if (HUDWidget) return; // Already initialized, skip

	const AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	HUDWidget = CreateWidget<UComplyHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
	HUDWidget->InitializeHUD(ASC);
	
	if (DamageNumbersWidget) return; // Already initialized, skip
	
	DamageNumbersWidget = CreateWidget<UDamageNumbersWidget>(this, DamageNumbersWidgetClass);
	DamageNumbersWidget->AddToViewport();
	
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	DamageNumbersWidget->SetPositionInViewport(FVector2D(0.f, 0.f));
	DamageNumbersWidget->SetDesiredSizeInViewport(ViewportSize);
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

void AComplyPlayerController::AddMappingContexts()
{
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

// Adds the widget and initializes it, and binds input for approving or denying the vote
void AComplyPlayerController::OnVoteKickInitiated(APlayerState* Target)
{
	if (!IsLocalController()) return;
	if (!VoteKickWidgetClass) return;

	VoteKickWidget = CreateWidget<UComplyVoteKickWidget>(this, VoteKickWidgetClass);
	if (VoteKickWidget)
	{
		VoteKickWidget->InitVote(Target);
		VoteKickWidget->AddToViewport(11);
	}
	
	InputComponent->BindKey(EKeys::F5, IE_Pressed, this, &AComplyPlayerController::OnVoteApprove);
	InputComponent->BindKey(EKeys::F10, IE_Pressed, this, &AComplyPlayerController::OnVoteDeny);
}

void AComplyPlayerController::Server_InitiateVoteKick_Implementation(APlayerState* Target)
{
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS)
	{
		GS->InitiateVoteKick(PlayerState, Target);
	}
}

void AComplyPlayerController::OnVoteApprove()
{
	Server_SubmitVote(PlayerState, true);
}

void AComplyPlayerController::OnVoteDeny()
{
	Server_SubmitVote(PlayerState, false);
}

void AComplyPlayerController::Server_SubmitVote_Implementation(APlayerState* Voter, bool bApprove)
{
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS)
	{
		GS->HandleVote(Voter, bApprove);
	}
}

// Ensures mapping contexts are added on the client whenever a pawn is possessed
void AComplyPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	AddMappingContexts();

	const AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(P);
	if (!PlayerCharacter) return;

	const AComplyPlayerState* PS = PlayerCharacter->GetPlayerState<AComplyPlayerState>();
	if (!PS) return;

	if (!HUDWidget)
	{
		HUDWidget = CreateWidget<UComplyHUDWidget>(this, HUDWidgetClass);
		HUDWidget->AddToViewport();
	}

	if (PS->GetAbilitySystemComponent())
	{
		HUDWidget->InitializeHUD(PS->GetAbilitySystemComponent());
	}
	
	if (!DamageNumbersWidget)
	{
		DamageNumbersWidget = CreateWidget<UDamageNumbersWidget>(this, DamageNumbersWidgetClass);
		DamageNumbersWidget->AddToViewport();
		
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		DamageNumbersWidget->SetPositionInViewport(FVector2D(0.f, 0.f));
		DamageNumbersWidget->SetDesiredSizeInViewport(ViewportSize);
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
		
		if (const AComplyGameModeBase* GM = GetWorld()->GetAuthGameMode<AComplyGameModeBase>())
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
