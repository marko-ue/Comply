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
void UDamageAbilityBase::CauseDamage(AActor* TargetActor, float ExplicitDamage, FComplyGameplayEffectContext* Context) const
{
	FGameplayEffectContextHandle ContextHandle(Context ? Context : new FComplyGameplayEffectContext());
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	DamageSpecHandle.Data->SetContext(ContextHandle);
    
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ExplicitDamage);
	
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*DamageSpecHandle.Data.Get(),
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor)
	);
}
