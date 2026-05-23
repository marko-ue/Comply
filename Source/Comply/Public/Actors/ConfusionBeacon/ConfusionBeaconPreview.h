// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConfusionBeaconPreview.generated.h"

UCLASS()
class COMPLY_API AConfusionBeaconPreview : public AActor
{
	GENERATED_BODY()

public:
	AConfusionBeaconPreview();
	
	void InitPreviewData(ACharacter* OwnerChar);
	void UpdatePosition();
	
	bool bCanPlace = false;

	virtual void Tick(float DeltaTime) override;
	
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
