// Copyright © 2026 Marko. All rights reserved.

#pragma once

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



