// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "ReloadAbility.generated.h"

class URangedWeaponAbilityBase;
/**
 * This ability is called on ranged weapons whenever reloading
 */
UCLASS()
class COMPLY_API UReloadAbility : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ReloadStateEffectClass;
	
	UPROPERTY()
	URangedWeaponAbilityBase* ActiveWeapon;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void HandleReload();
	
private:
	UFUNCTION()
	virtual void OnReloadMontageCompleted();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> ReloadMontageTask;
};
