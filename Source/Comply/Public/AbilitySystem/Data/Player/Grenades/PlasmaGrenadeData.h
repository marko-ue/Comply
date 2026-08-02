// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyGrenadeData.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "PlasmaGrenadeData.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class USoundCue;
/**
 * 
 */
UCLASS()
class COMPLY_API UPlasmaGrenadeData : public UComplyGrenadeData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float MaxDamage = 100.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Settings")
	float ExplosionRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ShieldOverlapDamageMultiplier = 0.5f;
};
