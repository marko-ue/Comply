// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ComplyPlayerController.generated.h"

class UComplyVoteKickWidget;
class UDamageNumbersWidget;
class UComplyHUDWidget;
class AComplyPlayerCharacter;
class UInputMappingContext;
class UUserWidget;

/**
 *
 */
UCLASS(abstract)
class AComplyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<UComplyHUDWidget> HUDWidget = nullptr;
	
	virtual void AcknowledgePossession(class APawn* P) override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UUserWidget> ActiveMenuWidget;
	
	// Functions called from objects that have menus (main menu, upgrade screen, mission debrief etc.)
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
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageNumbersWidget> DamageNumbersWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UDamageNumbersWidget> DamageNumbersWidget;
	
	void ShowFlashbangEffect();
	
	// These RPCs are used as wrappers for functions stored on the game state
	// They are required since widgets need to call the game state functions, but they can't call them directly
	// so instead they call these RPCs
	UFUNCTION(Server, Reliable)
	void Server_SubmitVote(APlayerState* Voter, bool bApprove);
	
	UFUNCTION(Server, Reliable)
	void Server_InitiateVoteKick(APlayerState* Target);
	
	UFUNCTION(Server, Reliable)
	void Server_SendChatMessage(const FString& Message);

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

private:
	void AddMappingContexts();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UComplyHUDWidget> HUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UComplyVoteKickWidget> VoteKickWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UComplyVoteKickWidget> VoteKickWidget;
	
	UFUNCTION()
	void OnVoteKickInitiated(APlayerState* Target);
	void OnVoteApprove();
	void OnVoteDeny();

	UFUNCTION()
	void TryBindGameStateEvents();
	
	FTimerHandle GameStateWaitTimerHandle;
	
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatMessage(const FString& PlayerName, const FString& Message);
};
