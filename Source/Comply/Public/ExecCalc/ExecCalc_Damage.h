// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

class UComplyDamageCalculationConfig;
/**
 * 
 */
UCLASS()
class COMPLY_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Damage();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TObjectPtr<UComplyDamageCalculationConfig> DamageConfig;
};
