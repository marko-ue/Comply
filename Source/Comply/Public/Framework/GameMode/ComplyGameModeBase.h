// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ComplyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API AComplyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	bool bFriendlyFire = false;
	
	UFUNCTION(BlueprintCallable)
	bool AllPlayersHaveUniqueCharacters();

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	UFUNCTION(BlueprintCallable)
	void TravelToMap(const FString& MapPath);
	
protected:
	virtual void BeginPlay() override;
};
