// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/DamageAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

/*
 * This function makes an outgoing gameplay effect spec that will be used in an execution calculation class
 * It will be called wherever damage is meant to be dealt, passing in the target actor and the custom context
 * The damage is passed in explicitly at call sites
 */
void UDamageAbilityBase::CauseDamage(AActor* TargetActor, float ExplicitDamage, FComplyGameplayEffectContext* Context)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ExplicitDamage);
	
	FComplyGameplayEffectContext* EffectContext = static_cast<FComplyGameplayEffectContext*>(
		DamageSpecHandle.Data->GetContext().Get());

	if (EffectContext && Context)
	{
		EffectContext->bHitThroughShield = Context->bHitThroughShield;
		EffectContext->ShieldDamageMultiplier = Context->ShieldDamageMultiplier;
	}

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}
