// Copyright © 2026 Marko. All rights reserved.


#include "ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "AbilitySystem/Data/ComplyDamageCalculationConfig.h"


struct ComplyDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	
	ComplyDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UComplyAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UComplyAttributeSet, ArmorPenetration, Source, false);
	}
};

static const ComplyDamageStatics& DamageStatics()
{
	static ComplyDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
											  FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.TargetTags = TargetTags;
	EvaluationParameters.SourceTags = SourceTags;

	const FComplyGameplayEffectContext* Context = static_cast<const FComplyGameplayEffectContext*>(
		Spec.GetContext().Get());

	const float PhysicalDamage = Spec.GetSetByCallerMagnitude(ComplyTags::ComplyAbilities::DamageTypes::Damage_Physical, false, 0.f);
	const float ElectricalDamage = Spec.GetSetByCallerMagnitude(ComplyTags::ComplyAbilities::DamageTypes::Damage_Electrical, false, 0.f);

	float Multiplier = 1.f;
	
	if (Context && Context->bHitThroughShield)
	{
		Multiplier *= Context->ShieldDamageMultiplier;
	}
	
	if (!DamageConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("UExecCalc_Damage: DamageConfig is null, damage will not be applied"));
		return;
	}
	
	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(0.f, Armor);
	
	float ArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().ArmorPenetrationDef, EvaluationParameters, ArmorPenetration
	);
	ArmorPenetration = FMath::Clamp(ArmorPenetration, 0.f, 1.f); // Must be clamped because if it's above 1 the formula breaks
	
	// Armor constant decides how fast armor damage reduction scales
	// Armor cap decides the highest possible damage reduction
	const float ArmorConstant = DamageConfig->ArmorConstant;
	const float ArmorCap = DamageConfig->ArmorReductionCap;
	
	// Effective armor is the original armor, reduced by 10% per 0.1 armor penetration
	const float EffectiveArmor = Armor * (1.f - ArmorPenetration);
	
	const float DamageReduction = FMath::Min(EffectiveArmor / (EffectiveArmor + ArmorConstant), ArmorCap);
	
	const float FinalPhysicalDamage = (PhysicalDamage * Multiplier) * (1.f - DamageReduction);
	const float FinalElectricalDamage = ElectricalDamage;
	
	const float FinalDamage = FinalPhysicalDamage + FinalElectricalDamage;

	const FGameplayModifierEvaluatedData EvaluatedData(
		UComplyAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage
	);
	
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
