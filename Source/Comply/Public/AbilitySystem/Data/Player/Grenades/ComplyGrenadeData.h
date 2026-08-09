// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ComplyGrenadeData.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyGrenadeData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> CostEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> NoThrowablesEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Settings")
	float ThrowSpeed = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade Settings")
	float FuseTime = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> GrenadeActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> PreviewGrenadeActor;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ThrowMontage;
	
	UPROPERTY(EditdefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> GrenadeMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> GrenadeMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> ChargesIcon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	TObjectPtr<UTexture2D> CrosshairTexture;
};
