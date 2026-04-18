// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Net/UnrealNetwork.h"

void UComplyAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// GAS attributes are set to replicate with no condition for replication, and to replicate whenever an attribute is received from the server, even when there's no value change
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UComplyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	/** 
	 * Clamps the health attribute to a minimum of 0 and maximum of max health
	 * This doesn't permanently clamp, and instead just changes the value returned from querying the modification of the attribute instead of actually setting health
	 * This means that further calculations will break the clamp, so clamping should also be done in PostGameplayEffectExecute
	 */
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UComplyAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	/** 
	 * Lowers health by the incoming damage meta attribute and clamps again
	 * The meta attribute is stored in a local variable and set to 0 at the start so the correct damage is applied. We also check if the damage is fatal
	 */
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0);
		if (LocalIncomingDamage > 0)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			UE_LOG(LogTemp, Warning, TEXT("Health after damage: %f"), GetHealth());
			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				Data.Target.GetAvatarActor()->Destroy();
			}
		}
	}
	
	// Clamps the attribute properly whenever a gameplay effect modifies it
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}

// In OnRep functions for GAS attributes, a specific GAS macro is used to that GAS handles replication and local prediction on its own
void UComplyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UComplyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}
