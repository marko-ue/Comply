// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ComplyGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API AComplyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// This replicated variable is used for clients to check if friendly fire is enabled. It's set internally by the GameMode
	UPROPERTY(Replicated)
	bool bFriendlyFire;
	
	// Used to globally track if all players have unique classes. Currently used so widgets can update their text without having to be constructed first
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bAllPlayersHaveUniqueClasses = false;
};
