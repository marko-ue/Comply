// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSelection.generated.h"

UCLASS()
class COMPLY_API ACharacterSelection : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACharacterSelection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
