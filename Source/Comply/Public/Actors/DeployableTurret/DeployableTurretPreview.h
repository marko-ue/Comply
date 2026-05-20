// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeployableTurretPreview.generated.h"

UCLASS()
class COMPLY_API ADeployableTurretPreview : public AActor
{
	GENERATED_BODY()

public:
	ADeployableTurretPreview();
	
	void InitPreviewData(ACharacter* OwnerChar);
	void UpdatePosition();
	
	virtual void Tick(float DeltaTime) override;
	
	bool bCanPlace = false;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> ValidMaterial;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> InvalidMaterial;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	ACharacter* OwnerCharacter;
};
