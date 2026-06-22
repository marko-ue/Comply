// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameMode/ComplyGameModeBase.h"

#include "ComplyPlayerController.h"
#include "EngineUtils.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Framework/GameState/ComplyGameStateBase.h"


void AComplyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS) GS->bFriendlyFire = bFriendlyFire;
}

// Checks if all players have unique characters selected to determine whether a mission can be started and other checks
bool AComplyGameModeBase::AllPlayersHaveUniqueCharacters()
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
