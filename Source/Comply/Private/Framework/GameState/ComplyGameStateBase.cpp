// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameState/ComplyGameStateBase.h"

#include "Framework/GameInstance/ComplyGameInstance.h"
#include "Net/UnrealNetwork.h"


void AComplyGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		const UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>();

		if (GI)
		{
			bFriendlyFire = GI->bFriendlyFire;
		}
	}
}

void AComplyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bFriendlyFire);
	DOREPLIFETIME(ThisClass, bAllPlayersHaveUniqueClasses);
}
