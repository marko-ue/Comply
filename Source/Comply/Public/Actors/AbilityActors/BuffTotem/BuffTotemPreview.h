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
