// Copyright Epic Games, Inc. All Rights Reserved.


#include "ComplyPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Comply.h"
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

		} else {

			UE_LOG(LogComply, Error, TEXT("Could not spawn mobile controls widget."));

		}

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
	InputMode.SetWidgetToFocus(ActiveMenuWidget->TakeWidget());
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
