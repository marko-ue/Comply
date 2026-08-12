// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ComplyGameModeBase.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSelectionChanged);

UCLASS()
class COMPLY_API AComplyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Settings")
	bool bFriendlyFire = false;
	
	UFUNCTION(BlueprintCallable)
	bool AllPlayersHaveUniqueCharacters() const;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UFUNCTION(BlueprintCallable)
	void TravelToMap(const FString& MapPath);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnPlayerSelectionChanged OnPlayerSelectionChanged;
	
protected:
	virtual void BeginPlay() override;
};
