// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameMode/ComplyGameModeBase.h"

#include "Framework/GameState/ComplyGameStateBase.h"

void AComplyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS) GS->bFriendlyFire = bFriendlyFire;
}
