// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/AbilityActors/ThrowablePreviewBase.h"
#include "DecoyGrenadePreview.generated.h"

UCLASS()
class COMPLY_API ADecoyGrenadePreview : public AThrowablePreviewBase
{
	GENERATED_BODY()

public:
	ADecoyGrenadePreview();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
