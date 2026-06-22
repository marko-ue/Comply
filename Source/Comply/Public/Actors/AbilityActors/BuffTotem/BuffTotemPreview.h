// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuffTotemPreview.generated.h"

UCLASS()
class COMPLY_API ABuffTotemPreview : public AActor
{
	GENERATED_BODY()

public:
	ABuffTotemPreview();
	
	void InitPreviewData(ACharacter* OwnerChar);
	
	virtual void Tick(float DeltaTime) override;
	
	void UpdatePosition();
	
	bool bCanPlace = false;
	
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
