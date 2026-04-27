// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "RangedWeaponAbilityBase.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitDelay;
class UHitscanTargetData;
class UCameraComponent;
/**
 * 
 */

UENUM(BlueprintType)
enum class ERangedWeaponType : uint8
{
	Automatic,
	Manual
};

UCLASS(Abstract)
class COMPLY_API URangedWeaponAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AbilityActivationMontageHip;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AbilityActivationMontageIronsights;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	void TraceToCrosshair(FHitResult& TraceHitResult, float TraceDistance, bool& OutPassedThroughShield);
	
	UPROPERTY(EditAnywhere)
	float TraceDistance = 10000.f;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere)
	float FireInterval = 60.f;
	
	UPROPERTY(EditDefaultsOnly)
	ERangedWeaponType RangedWeaponType = ERangedWeaponType::Automatic;
	
	UPROPERTY(EditAnywhere, Category = "Upgrades")
	float ShieldShotDamageMultiplier = 1.5f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReloadEffectClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReloadStateEffectClass;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FVector Start;
	
	UPROPERTY(EditDefaultsOnly)
	FVector End;
	
	UFUNCTION()
	virtual void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	/*
	 * Functions for the delay task and firing
	 */
	UFUNCTION()
	virtual void OnFireDelayFinished();
	
	virtual bool Fire();
	// End
	
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay);
	virtual void PlayAnimationBasedOnState(); 
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> FireDelayTask;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReloadMontageTask;
	
	UFUNCTION()
	virtual void OnReloadMontageCompleted();
	
	// Commits ability cost (spends ammo) and plays a montage if the mag is empty. The callback applies the reload GE
	virtual bool SpendAmmoAndPlayMontageIfEmpty();

	
private:
	UPROPERTY()
	UHitscanTargetData* HitscanTargetDataTask;
};
