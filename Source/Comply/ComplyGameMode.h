// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ComplyGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AComplyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AComplyGameMode();
};



