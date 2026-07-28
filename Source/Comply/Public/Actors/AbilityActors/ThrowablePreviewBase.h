// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowablePreviewBase.generated.h"

UCLASS(Abstract)
class COMPLY_API AThrowablePreviewBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;
	UPROPERTY()
	APawn* OwningPawn = nullptr;
	float ThrowSpeed = 0.f;
};
