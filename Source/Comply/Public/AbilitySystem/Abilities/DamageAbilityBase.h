// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "ComplyAbilityBase.h"

#include "DamageAbilityBase.generated.h"

struct FComplyGameplayEffectContext;

/**
 * Base class for all abilities that do damage.
 * Has a method that damage abilities can call to do damage with parameters.
 */
UCLASS(Abstract)
class COMPLY_API UDamageAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	void CauseDamage(AActor* TargetActor, float ExplicitDamage, FComplyGameplayEffectContext* Context = nullptr);

	UPROPERTY()
	FGameplayTag DamageType;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
