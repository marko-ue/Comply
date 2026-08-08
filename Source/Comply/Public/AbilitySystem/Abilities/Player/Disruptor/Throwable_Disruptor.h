// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Throwable_Disruptor.generated.h"

class UDecoyGrenadeData;
/**
 * 
 */
UCLASS()
class COMPLY_API UThrowable_Disruptor : public UThrowableAbilityBase
{
	GENERATED_BODY()

public:
	FORCEINLINE virtual FGameplayAttribute GetCurrentChargesAttribute() const override { return UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute(); }
	FORCEINLINE virtual FGameplayAttribute GetMaxChargesAttribute() const override { return UWeaponAttributeSet::GetDecoyGrenadeMaxChargesAttribute(); }
	
protected:
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual void CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute() override;
};
