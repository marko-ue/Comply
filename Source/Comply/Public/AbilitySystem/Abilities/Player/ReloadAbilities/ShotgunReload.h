// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/ComplyAbilityBase.h"

#include "ShotgunReload.generated.h"

class URangedWeaponAbilityBase;
class UAbilityTask_WaitGameplayTagAdded;

/**
 * Reload ability used by the shotgun. Reloads 1 shell at a time.
 */
UCLASS()
class COMPLY_API UShotgunReload : public UComplyAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	void LoadNextShell();
    
	UFUNCTION()
	void OnShellMontageCompleted();

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ShellMontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayTagAdded> WaitFiringTagTask;

	UPROPERTY()
	TObjectPtr<URangedWeaponAbilityBase> ActiveWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> AddAmmoEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ReloadStateEffectClass;
};
