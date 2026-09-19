// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"

// UE
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"

#include "ElectricHazardZoneData.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class UNiagaraComponent;
class UAudioComponent;
class USoundBase;

/**
 * Data asset used by the electric hazard zone containing variables for the damage and stun.
 */
UCLASS()
class COMPLY_API UElectricHazardZoneData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> StunEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ElectricEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> ElectricSound;
};
