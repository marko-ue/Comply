// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Actors/UtilityActors/UtilityPreviewBase.h"

#include "BuffTotemPreview.generated.h"

/**
 * Buff totem preview that shows up when clicking once but not confirming input. Shows different material based on if placement is valid or not.
 */
UCLASS()
class COMPLY_API ABuffTotemPreview : public AUtilityPreviewBase
{
	GENERATED_BODY()

public:
	ABuffTotemPreview();

	virtual void Tick(float DeltaTime) override;
	virtual void InitPreviewData(ACharacter* OwnerChar, const UComplyUtilityData* InUtilityData = nullptr) override;
	
	void UpdatePosition();
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BuffTotemMesh;
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> ValidMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> InvalidMaterial;

protected:
	virtual void BeginPlay() override;
};
