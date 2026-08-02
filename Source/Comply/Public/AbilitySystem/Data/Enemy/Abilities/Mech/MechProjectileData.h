// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/Enemy/Abilities/ComplyEnemyAbilityData.h"
#include "MechProjectileData.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class AMechProjectileAreaEffect;
/**
 * 
 */
UCLASS()
class COMPLY_API UMechProjectileData : public UComplyEnemyAbilityData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AMechProjectileAreaEffect> AreaEffectActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> ProjectileMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> ProjectileMaterial;
	
	UPROPERTY(EDitDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> AreaEffectMesh;
	
	UPROPERTY(EDitDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> AreaEffectMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> SlowEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Area Effect Settings")
	float AreaEffectRadius = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> AreaEffectSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> AreaEffectParticles;
};
