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

	UPROPERTY()
	FGameplayTag DamageType;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
