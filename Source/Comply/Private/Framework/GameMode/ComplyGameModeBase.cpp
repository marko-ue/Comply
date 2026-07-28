// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameMode/ComplyGameModeBase.h"

#include "ComplyPlayerController.h"
#include "EngineUtils.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "Framework/PlayerState/ComplyPlayerState.h"


void AComplyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS) GS->bFriendlyFire = bFriendlyFire;
}

// Checks if all players have unique characters selected to determine whether a mission can be started and other checks
bool AComplyGameModeBase::AllPlayersHaveUniqueCharacters() const
{
	TArray<TSubclassOf<AComplyPlayerCharacter>> SelectedClasses;

	for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
	{
		const AComplyPlayerController* ComplyPC = Cast<AComplyPlayerController>(PC);
		if (!ComplyPC) continue;

		// If a player hasn't selected a character, block travel too
		if (!ComplyPC->SelectedCharacterClass)
			return false;

		if (SelectedClasses.Contains(ComplyPC->SelectedCharacterClass))
			return false; // Duplicate found, block travel

		SelectedClasses.Add(ComplyPC->SelectedCharacterClass);
	}

	return true;
}

void AComplyGameModeBase::TravelToMap(const FString& MapPath)
{
	UE_LOG(LogTemp, Warning, TEXT("TravelToMap called, bUseSeamlessTravel: %d"), bUseSeamlessTravel);
	GetWorld()->ServerTravel(MapPath);
}

// Returns the player's selected character class from the GameInstance instead of the GameMode default
UClass* AComplyGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (AComplyPlayerController* PC = Cast<AComplyPlayerController>(InController))
	{
		if (UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>())
		{
			if (AComplyPlayerState* PS = PC->GetPlayerState<AComplyPlayerState>())
			{
				FString PlayerID = FString::FromInt(PS->GetPlayerId());
				if (TSubclassOf<AComplyPlayerCharacter>* Found = GI->PlayerCharacterSelections.Find(PlayerID))
				{
					if (*Found) return *Found;
				}
			}
		}
	}
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
