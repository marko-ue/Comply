// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
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
	
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumCurrentAmmoAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumMaxAmmoAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const override { return ComplyTags::SetByCaller::SBC_ReduceMagnumReserveAmmo; }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumCurrentReserveAmmoAttribute(); }
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool Fire() override;
	virtual void PlayAnimationBasedOnState() override;
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay) override;
	virtual void OnMontageCancelled() override;
	virtual void OnMontageCompleted() override;
};
