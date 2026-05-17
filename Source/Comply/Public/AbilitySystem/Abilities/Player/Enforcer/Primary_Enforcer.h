// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Primary_Enforcer.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UPrimary_Enforcer : public URangedWeaponAbilityBase
{
	GENERATED_BODY()
	
public:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool Fire() override;
	virtual void PlayAnimationBasedOnState() override;
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay) override;
	virtual void OnMontageCancelled() override;
	virtual void OnMontageCompleted() override;
};
