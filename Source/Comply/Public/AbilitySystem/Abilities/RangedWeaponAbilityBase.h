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
	
	virtual void Fire();
	// End
	
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay);
	virtual void PlayAnimationBasedOnState();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> FireDelayTask;

	
private:
	UPROPERTY()
	UHitscanTargetData* HitscanTargetDataTask;
};
