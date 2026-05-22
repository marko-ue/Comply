// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "DamageAbilityBase.generated.h"

struct FComplyGameplayEffectContext;
/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UDamageAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	void CauseDamage(AActor* TargetActor, float ExplicitDamage, FComplyGameplayEffectContext* Context = nullptr);
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;
	
	// A damage gameplay effect that will use an execution calculation.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
