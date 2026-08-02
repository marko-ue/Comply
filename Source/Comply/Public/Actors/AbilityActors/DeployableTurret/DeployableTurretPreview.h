// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeployableTurretPreview.generated.h"

class UDeployableTurretAbilityData;
class UArrowComponent;

UCLASS()
class COMPLY_API ADeployableTurretPreview : public AActor
{
	GENERATED_BODY()

public:
	ADeployableTurretPreview();
	
	void InitPreviewData(ACharacter* OwnerChar, const UDeployableTurretAbilityData* InTurretData);
	
	virtual void Tick(float DeltaTime) override;
	
	void UpdatePosition();
	
	bool bCanPlace = false;
	bool bShouldUpdatePosition = true;
	
	UPROPERTY()
	FHitResult LastPlacementHit;
	FVector PlacementLocation;
	FRotator PlacementRotation;
	
	// Constructs the static mesh component for the turret mesh. Actual mesh assigned through data asset
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TurretMesh;
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> ValidMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> InvalidMaterial;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	ACharacter* OwnerCharacter;
};
