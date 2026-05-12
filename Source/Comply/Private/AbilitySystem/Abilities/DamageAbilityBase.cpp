// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/DamageAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

/*
 * This function makes an outgoing gameplay effect spec that will be used in an execution calculation class
 * It will be called wherever damage is meant to be dealt, passing in the target actor and the custom context
 * The base scaled damage is set through blueprint using a curve
 */
void UDamageAbilityBase::CauseDamage(AActor* TargetActor, FComplyGameplayEffectContext* Context)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);

	if (Context)
	{
		DamageSpecHandle.Data->SetContext(FGameplayEffectContextHandle(Context));
	}

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}
