// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ComplyAbilityBase.h"
#include "ShotgunReload.generated.h"

class URangedWeaponAbilityBase;
class UAbilityTask_WaitGameplayTagAdded;
/**
 * 
 */
UCLASS()
class COMPLY_API UShotgunReload : public UComplyAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	void LoadNextShell();
    
	UFUNCTION()
	void OnShellMontageCompleted();

	UFUNCTION()
	void OnFiringTagAdded();

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
