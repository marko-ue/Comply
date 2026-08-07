// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/Data/Player/Weapons/ComplyWeaponTypes.h"
#include "ComplyWeaponData.generated.h"


class UNiagaraSystem;
class UGameplayAbility;
class UGameplayEffect;
class UComplyDamageData;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> AbilityActivationMontageHip;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> AbilityActivationMontageIronsights;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> InsertShellMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ReduceAmmoEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ReduceReserveAmmoEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ReloadEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TSubclassOf<UGameplayAbility> ReloadAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	float TraceDistance = 10000.f;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	float FireInterval = 60.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	ERangedWeaponType RangedWeaponType;
	
	// If the ranged weapon uses a simple line trace to the crosshair
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	bool bUsesSingleCrosshairTrace = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	float ShieldShotDamageMultiplier = 1.5f;

	// Damage data asset for setting each weapon's damage, damage type, etc.
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TObjectPtr<UComplyDamageData> DamageData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TArray<TObjectPtr<UMaterialInstance>> BulletImpactDecals;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> BulletTracerEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	TSubclassOf<UCameraShakeBase> FiringCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	float RecoilKickDistance = 6.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	float RecoilKickSpeed = 12.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UTexture2D> BulletIcon;
};
