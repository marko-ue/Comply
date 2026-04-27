// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UWeaponAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// GAS attributes are set to replicate with no condition for replication, and to replicate whenever an attribute is received from the server, even when there's no value change
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RifleCurrentAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RifleMaxAmmo, COND_None, REPNOTIFY_Always);
}

void UWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetRifleCurrentAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetRifleMaxAmmo());
	}
}

void UWeaponAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// Clamps the attribute properly whenever a gameplay effect modifies it
	if (Data.EvaluatedData.Attribute == GetRifleCurrentAmmoAttribute())
	{
		SetRifleCurrentAmmo(FMath::Clamp(GetRifleCurrentAmmo(), 0.f, GetRifleMaxAmmo()));
	}
}

// In OnRep functions for GAS attributes, a specific GAS macro is used to that GAS handles replication and local prediction on its own
void UWeaponAttributeSet::OnRep_RifleCurrentAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, RifleCurrentAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_RifleMaxAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, RifleMaxAmmo, OldValue);
}
