// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Primary_Disruptor.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UPrimary_Disruptor : public URangedWeaponAbilityBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE virtual FGameplayAttribute GetCurrentAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunCurrentAmmoAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunMaxAmmoAttribute(); }
	FORCEINLINE virtual FGameplayTag GetReduceReserveAmmoTag() const override { return ComplyTags::SetByCaller::SBC_ReduceShotgunReserveAmmo; }
	FORCEINLINE virtual FGameplayAttribute GetCurrentReserveAmmoAttribute() const override { return UWeaponAttributeSet::GetShotgunCurrentReserveAmmoAttribute(); }
	
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	int32 NumberOfPellets = 8;
	
	// TODO: Turn into a scalable float for upgrades
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float SpreadAngle = 75.f;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual bool Fire() override;
	
	virtual void PlayAnimationBasedOnState() override;
	virtual void PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay) override;
	virtual void OnMontageCancelled() override;
	virtual void OnMontageCompleted() override;
};
