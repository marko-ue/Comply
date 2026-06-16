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
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> AbilityActivationMontageHip;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> AbilityActivationMontageIronsights;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> InsertShellMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|Reloading")
	TSubclassOf<UGameplayEffect> ReduceAmmoEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|Reloading")
	TSubclassOf<UGameplayEffect> ReduceReserveAmmoEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|Reloading")
	TSubclassOf<UGameplayAbility> ReloadAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|Reloading")
	TSubclassOf<UGameplayEffect> ReloadEffectClass;
	
	UPROPERTY(EditAnywhere, Category = "Ability Properties|Trace")
	float TraceDistance = 10000.f;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float FireInterval = 60.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Properties|Types")
	ERangedWeaponType RangedWeaponType = ERangedWeaponType::Automatic;
	
	UPROPERTY(EditAnywhere, Category = "Ability Properties|Upgrades")
	float ShieldShotDamageMultiplier = 1.5f;
	
	// If the ranged weapon uses a simple line trace to the crosshair
	UPROPERTY(EditDefaultsOnly, Category = "Ability Properties|Types")
	bool bUsesSingleCrosshairTrace = true;
	
	// Derived weapon classes override these to expose their own attributes and tags
	// The base class uses them to handle reloading generically
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const { return FGameplayTag(); }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual bool DoesWeaponUseCrosshairTrace() const { return bUsesSingleCrosshairTrace; }

protected:
	virtual bool Fire();
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Properties|Trace")
	FVector Start;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Properties|Trace")
	FVector End;
	
	UFUNCTION()
	virtual void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	// Function for the delay task
	UFUNCTION()
	virtual void OnFireDelayFinished();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> FireDelayTask;
	
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay);
	virtual void PlayAnimationBasedOnState();
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY()
	TObjectPtr<URangedWeaponAbilityBase> ActiveWeapon;
	
	UPROPERTY()
	TObjectPtr<UHitscanTargetData> HitscanTargetDataTask;
};
