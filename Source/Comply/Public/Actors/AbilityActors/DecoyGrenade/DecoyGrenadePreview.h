// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DecoyGrenadePreview.generated.h"

UCLASS()
class COMPLY_API ADecoyGrenadePreview : public AActor
{
	GENERATED_BODY()

public:
	ADecoyGrenadePreview();
	
	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	
	UPROPERTY()
	TObjectPtr<APawn> OwningPawn;
	
	float ThrowSpeed;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
