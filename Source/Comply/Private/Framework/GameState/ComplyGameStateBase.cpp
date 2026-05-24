// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameState/ComplyGameStateBase.h"

#include "Net/UnrealNetwork.h"

void AComplyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bFriendlyFire);
}
