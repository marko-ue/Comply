// Copyright © 2026 Marko. All rights reserved.


#include "ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAttributeSet.h"

struct AuraDamageStatics
{
};

UExecCalc_Damage::UExecCalc_Damage()
{
}

// TODO: When adding new damage types, make it so the set by caller tag can be passed in, as well as any resistances/advantages against that damage type, and general resistance attributes like Armor
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	float Damage = Spec.GetSetByCallerMagnitude(ComplyTags::ComplyAbilities::DamageTypes::Damage_Physical);
	
	const FGameplayModifierEvaluatedData EvaluatedData(UComplyAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
