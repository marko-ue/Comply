// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "RangedWeaponAbilityBase.generated.h"

class UWeaponAttributeSet;
class UReloadAbility;
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
	SemiAutomatic
};

UCLASS(Abstract)
class COMPLY_API URangedWeaponAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	void TraceToCrosshair(FHitResult& TraceHitResult, const float TraceLength, bool& OutPassedThroughShield);
	void PerformShotgunTraces(TArray<FHitResult>& OutHitResults, const int32 NumPellets, const float TraceLength, bool& OutPassedThroughShield);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AbilityActivationMontageHip;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AbilityActivationMontageIronsights;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReduceAmmoEffectClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReduceReserveAmmoEffectClass;
	
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
	TSubclassOf<UGameplayAbility> ReloadAbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReloadEffectClass;
	
	// If the ranged weapon uses a simple line trace to the crosshair
	UPROPERTY(EditDefaultsOnly)
	bool bUsesSingleCrosshairTrace = true;
	
	// Each weapon will return its own data
	// This is mostly used to handle reloading more cleanly
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const { return FGameplayTag(); }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual bool DoesWeaponUseCrosshairTrace() const { return bUsesSingleCrosshairTrace; }
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> InsertShellMontage;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
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

private:
	UPROPERTY()
	UHitscanTargetData* HitscanTargetDataTask;
	
	UPROPERTY()
	URangedWeaponAbilityBase* ActiveWeapon;
};
