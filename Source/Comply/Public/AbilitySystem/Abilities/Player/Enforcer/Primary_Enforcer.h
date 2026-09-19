// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"

// Comply
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "AbilitySystem/ComplyTags.h"

#include "Primary_Enforcer.generated.h"

/**
 * Revolver (magnum) type weapon (semi-automatic, high damage, single target).
 */
UCLASS()
class COMPLY_API UPrimary_Enforcer : public URangedWeaponAbilityBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumCurrentAmmoAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumMaxAmmoAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const override { return ComplyTags::SetByCaller::SBC_ReduceMagnumReserveAmmo; }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumCurrentReserveAmmoAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxReserveAmmoAttribute() const override { return UWeaponAttributeSet::GetMagnumMaxReserveAmmoAttribute(); }

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool Fire() override;
};
