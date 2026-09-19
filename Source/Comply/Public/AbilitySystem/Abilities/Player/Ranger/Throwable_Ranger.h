// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"

// Comply
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"


#include "Throwable_Ranger.generated.h"

class UPlasmaGrenadeData;

/**
 * Plasma grenade that explodes after a delay and deals falloff damage to enemies in a radius.
 */
UCLASS()
class COMPLY_API UThrowable_Ranger : public UThrowableAbilityBase
{
	GENERATED_BODY()

public:
	FORCEINLINE virtual FGameplayAttribute GetCurrentChargesAttribute() const override { return UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxChargesAttribute() const override { return UWeaponAttributeSet::GetPlasmaGrenadeMaxChargesAttribute(); }
	
protected:
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual void CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute() override;
};
