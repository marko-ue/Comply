// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ComplyGameInstance.generated.h"

class AComplyPlayerCharacter;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Character selections are stored in the GameInstance rather than PlayerState
	// because seamless travel player migration is unreliable for the listen server host,
	// whose controller is recreated via PostLogin rather than migrated via InitSeamlessTravelPlayer.
	// GameInstance persists across all travels and is accessible to all server-side systems
	UPROPERTY()
	TMap<FString, TSubclassOf<AComplyPlayerCharacter>> PlayerCharacterSelections;
};
