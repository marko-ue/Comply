// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Primary_Disruptor.generated.h"

class UShotgunWeaponData;
/**
 * 
 */
UCLASS()
class COMPLY_API UPrimary_Disruptor : public URangedWeaponAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UShotgunWeaponData> ShotgunWeaponData;
	
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunCurrentAmmoAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunMaxAmmoAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const override { return ComplyTags::SetByCaller::SBC_ReduceShotgunReserveAmmo; }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunCurrentReserveAmmoAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxReserveAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunMaxReserveAmmoAttribute(); }
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool Fire() override;
};
