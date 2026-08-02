// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComplyUtilityData.generated.h"

class AUtilityPreviewBase;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyUtilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> UtilityActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AUtilityPreviewBase> UtilityPreviewActorClass;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditDefaultsOnly, Category = "Utility Settings")
	float UtilityLifetime = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> PlaceUtilityMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> UtilityMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> UtilityPreviewMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> UtilityMaterial;
};
