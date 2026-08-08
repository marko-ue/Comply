// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "AbilitySystem/Data/Player/Weapons/ComplyWeaponData.h"
#include "RangedWeaponAbilityBase.generated.h"

class AComplyCharacterBase;
class UNiagaraSystem;
class UWeaponAttributeSet;
class UReloadAbility;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitDelay;
class UHitscanTargetData;
class UCameraComponent;
/**
 * 
 */

UCLASS(Abstract)
class COMPLY_API URangedWeaponAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyWeaponData> WeaponData;
	
	void TraceToCrosshair(FHitResult& TraceHitResult, const float TraceLength, bool& OutPassedThroughShield);
	void PerformShotgunTraces(TArray<FHitResult>& OutHitResults, const int32 NumPellets, const float TraceLength, bool& OutPassedThroughShield);
	
	// Derived weapon classes override these to expose their own attributes and tags
	// The base class uses them to handle reloading generically
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const { return FGameplayTag(); }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxReserveAmmoAttribute() const { return FGameplayAttribute(); }
	FORCEINLINE virtual bool DoesWeaponUseCrosshairTrace() const { return WeaponData->bUsesSingleCrosshairTrace; }
	
	UPROPERTY()
	TObjectPtr<URangedWeaponAbilityBase> ActiveWeapon;

protected:
	// Tracks shots fired but not yet confirmed by the server, used to offset
	// CheckCost on the client for automatic weapons since cost isn't predicted
	int32 PendingShotCount = 0;
	FDelegateHandle AmmoReplicatedHandle;
	
	void BuildWeaponCollisionParams(const AActor* Avatar, FCollisionQueryParams& OutQueryParams, FCollisionObjectQueryParams& OutObjectParams);
	
	virtual bool Fire();

	UFUNCTION()
	virtual void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	// Function for the delay task
	UFUNCTION()
	virtual void OnFireDelayFinished();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> FireDelayTask;
	
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay);
	virtual void PlayAnimationBasedOnState();
	
	virtual void OnMontageCompleted() override;
	virtual void OnMontageCancelled() override;
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY()
	TObjectPtr<UHitscanTargetData> HitscanTargetDataTask;
};
