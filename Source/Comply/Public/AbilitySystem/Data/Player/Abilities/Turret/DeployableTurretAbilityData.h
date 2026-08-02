// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DeployableTurretAbilityData.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class USoundCue;
/**
 * 
 */
UCLASS()
class COMPLY_API UDeployableTurretAbilityData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditDefaultsOnly, Category = "Turret Settings")
	float TurretLifetime = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> TurretActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> TurretPreviewActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> PlaceTurretMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> TurretMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> TurretMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> ValidMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> InvalidMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageTypeTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> PlaceTurretParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> TurretFireSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Turret Settings")
	int32 RechargeAmount = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> RechargeTurretChargeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> BulletTracerEffect;
};
