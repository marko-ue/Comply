// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "DamageAbilityBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UDamageAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	void CauseDamage(AActor* TargetActor) const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;
	
	// A damage gameplay effect that will use an execution calculation.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
