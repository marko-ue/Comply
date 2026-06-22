// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ComplyPlayerController.generated.h"

class AComplyPlayerCharacter;
class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AComplyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUserWidget> ActiveMenuWidget;
	
	// Functions called from objects that have menus (main menu, interactables, upgrade screen, mission debrief etc.)
	UFUNCTION(BlueprintCallable)
	void OpenMenuWidget(TSubclassOf<UUserWidget> WidgetClass);
	UFUNCTION(BlueprintCallable)
	void CloseMenuWidget();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_SelectCharacter(TSubclassOf<AComplyPlayerCharacter> SelectedCharacter);
	
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AComplyPlayerCharacter> SelectedCharacterClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AComplyPlayerCharacter> DefaultCharacterClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> FlashbangWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> FlashbangWidget;
	
	void ShowFlashbangEffect();

	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;
	
	virtual void OnRep_PlayerState() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

};
