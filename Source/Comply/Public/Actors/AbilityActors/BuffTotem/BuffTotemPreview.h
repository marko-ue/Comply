// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/UtilityActors/UtilityPreviewBase.h"
#include "BuffTotemPreview.generated.h"

UCLASS()
class COMPLY_API ABuffTotemPreview : public AUtilityPreviewBase
{
	GENERATED_BODY()

public:
	ABuffTotemPreview();

	virtual void Tick(float DeltaTime) override;
	
	void UpdatePosition();

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> ValidMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> InvalidMaterial;

protected:
	virtual void BeginPlay() override;
};
