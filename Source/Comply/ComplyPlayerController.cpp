// Copyright Epic Games, Inc. All Rights Reserved.


#include "ComplyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Comply.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "Framework/GameMode/ComplyGameModeBase.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "UI/Widgets/ComplyHUDWidget.h"
#include "UI/Widgets/ComplyRevivePromptWidget.h"
#include "UI/Widgets/ComplyVoteKickWidget.h"
#include "UI/Widgets/Chat/ComplyChatBoxWidget.h"
#include "UI/Widgets/DamageNumbers/DamageNumbersWidget.h"
#include "Widgets/Input/SVirtualJoystick.h"


AComplyPlayerController::AComplyPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

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
	
	if (ChatBoxWidgetClass)
	{
		ChatBoxWidget = CreateWidget<UComplyChatBoxWidget>(this, ChatBoxWidgetClass);
		ChatBoxWidget->AddToViewport(10);
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

	const AComplyPlayerState* PS = GetPlayerState<AComplyPlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	const FComplyHUDLayout* Layout = nullptr;
	if (SelectedCharacterClass)
	{
		if (const AComplyPlayerCharacter* CDO = SelectedCharacterClass->GetDefaultObject<AComplyPlayerCharacter>())
		{
			if (CDO->PlayerData) Layout = &CDO->PlayerData->HUDLayout;
		}
	}

	TryInitializeHUD(ASC, Layout);
	TryInitializeDamageNumbers();
}

void AComplyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TickRevivePromptCheck();
}

void AComplyPlayerController::TickRevivePromptCheck()
{
	if (!IsLocalController()) return;
	if (!RevivePromptWidget) return;
    
	const AComplyPlayerCharacter* LocalPlayer = Cast<AComplyPlayerCharacter>(GetPawn());
	if (!LocalPlayer) return;

	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(
	   this, LocalPlayer, 250, TraceStart, TraceEnd, TraceDirection))
	{
		return;
	}

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(LocalPlayer);

	FHitResult Hit;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(100.f);
	GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_Interact, Sphere, QueryParams);

	AComplyPlayerCharacter* HitPlayer = Cast<AComplyPlayerCharacter>(Hit.GetActor());
	AComplyPlayerCharacter* NewHovered = (HitPlayer && HitPlayer->bIsDowned) ? HitPlayer : nullptr;

	if (NewHovered != HoveredDownedPlayer)
	{
		HoveredDownedPlayer = NewHovered;
		RevivePromptWidget->SetVisibility(NewHovered ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	}
}

// Ensures mapping contexts are added on the client whenever a pawn is possessed
void AComplyPlayerController::AcknowledgePossession(class APawn* P)
{
	Super::AcknowledgePossession(P);

	AddMappingContexts();

	AComplyPlayerCharacter* ComplyCharacter = Cast<AComplyPlayerCharacter>(P);
	if (!ComplyCharacter) return;

	const AComplyPlayerState* PS = ComplyCharacter->GetPlayerState<AComplyPlayerState>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	const FComplyHUDLayout* Layout = ComplyCharacter->PlayerData ? &ComplyCharacter->PlayerData->HUDLayout : nullptr;

	TryInitializeHUD(ASC, Layout);
	TryInitializeDamageNumbers();
}

void AComplyPlayerController::TryInitializeHUD(UAbilitySystemComponent* ASC, const FComplyHUDLayout* Layout)
{
	if (HUDWidget) return;

	const FString MapName = GetWorld()->GetMapName();
	if (MapName.Contains("Lobby")) return;

	HUDWidget = CreateWidget<UComplyHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
	HUDWidget->InitializeHUD(ASC);

	if (Layout)
	{
		HUDWidget->InitializeLayout(*Layout);
	}

	InitializeRevivePrompt();
}

void AComplyPlayerController::TryInitializeDamageNumbers()
{
	if (DamageNumbersWidget) return;

	const FString MapName = GetWorld()->GetMapName();
	if (MapName.Contains("Lobby")) return;

	DamageNumbersWidget = CreateWidget<UDamageNumbersWidget>(this, DamageNumbersWidgetClass);
	DamageNumbersWidget->AddToViewport();

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	if (ViewportSize.IsZero())
	{
		GetWorld()->GetTimerManager().SetTimer(DamageNumbersSizeRetryHandle, [this]()
		{
			if (!DamageNumbersWidget) return;

			FVector2D Size;
			GetWorld()->GetGameViewport()->GetViewportSize(Size);
			if (Size.IsZero()) return;

			DamageNumbersWidget->SetPositionInViewport(FVector2D(0.f, 0.f));
			DamageNumbersWidget->SetDesiredSizeInViewport(Size);
			GetWorld()->GetTimerManager().ClearTimer(DamageNumbersSizeRetryHandle);
		}, 0.1f, true);
		return;
	}

	DamageNumbersWidget->SetPositionInViewport(FVector2D(0.f, 0.f));
	DamageNumbersWidget->SetDesiredSizeInViewport(ViewportSize);
}

void AComplyPlayerController::InitializeRevivePrompt()
{
	// Create revive prompt directly on the PC
	RevivePromptWidget = CreateWidget<UComplyRevivePromptWidget>(this, RevivePromptWidgetClass);
	RevivePromptWidget->AddToViewport(5);
	RevivePromptWidget->SetVisibility(ESlateVisibility::Hidden);
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

// The server RPC is called by the widget that calls a client RPC which handles showing the message to every client
void AComplyPlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	const FString TrimmedMessage = Message.TrimStartAndEnd();
	if (TrimmedMessage.IsEmpty()) return;

	const FString SenderName = PlayerState ? PlayerState->GetPlayerName() : TEXT("Unknown");

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS) return;

	for (const APlayerState* PS : GS->PlayerArray)
	{
		if (AComplyPlayerController* PC = Cast<AComplyPlayerController>(PS->GetPlayerController()))
		{
			PC->Client_ReceiveChatMessage(SenderName, TrimmedMessage);
		}
	}
}

// Adds the message to the chat box widget
void AComplyPlayerController::Client_ReceiveChatMessage_Implementation(const FString& PlayerName, const FString& Message)
{
	if (ChatBoxWidget)
	{
		ChatBoxWidget->AddMessage(PlayerName, Message);
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
