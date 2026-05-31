// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ComplyAbilityBase.h"
#include "ReloadAbility.generated.h"

class URangedWeaponAbilityBase;
/**
 * This ability is called on some ranged weapons whenever reloading
 */
UCLASS()
class COMPLY_API UReloadAbility : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ReloadStateEffectClass;
	
	UPROPERTY()
	URangedWeaponAbilityBase* ActiveWeapon;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void HandleReload();
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	virtual void OnReloadMontageCompleted();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReloadMontageTask;
};
