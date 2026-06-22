// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeployableTurretPreview.generated.h"

class UArrowComponent;

UCLASS()
class COMPLY_API ADeployableTurretPreview : public AActor
{
	GENERATED_BODY()

public:
	ADeployableTurretPreview();
	
	void InitPreviewData(ACharacter* OwnerChar);
	
	virtual void Tick(float DeltaTime) override;
	
	void UpdatePosition();
	
	bool bCanPlace = false;
	bool bShouldUpdatePosition = true;
	
	FVector PlacementLocation;
	FRotator PlacementRotation;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TurretMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> ValidMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> InvalidMaterial;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	ACharacter* OwnerCharacter;
};
