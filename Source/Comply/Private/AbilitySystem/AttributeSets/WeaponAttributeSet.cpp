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
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RifleCurrentReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RifleMaxReserveAmmo, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PlasmaGrenadeCurrentCharges, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PlasmaGrenadeMaxCharges, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagnumCurrentAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagnumMaxAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagnumCurrentReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagnumMaxReserveAmmo, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, TurretCurrentCharges, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, TurretMaxCharges, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ShotgunCurrentAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ShotgunMaxAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ShotgunCurrentReserveAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ShotgunMaxReserveAmmo, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DecoyGrenadeCurrentCharges, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DecoyGrenadeMaxCharges, COND_None, REPNOTIFY_Always);
}

void UWeaponAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetRifleCurrentAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetRifleMaxAmmo());
	}
	if (Attribute == GetRifleCurrentReserveAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetRifleMaxReserveAmmo());
	}
	
	if (Attribute == GetPlasmaGrenadeCurrentChargesAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetPlasmaGrenadeMaxCharges());
	}
	
	if (Attribute == GetMagnumCurrentAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMagnumMaxAmmo());
	}
	if (Attribute == GetMagnumCurrentReserveAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMagnumMaxReserveAmmo());
	}
	
	if (Attribute == GetTurretCurrentChargesAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetTurretMaxCharges());
	}
	
	if (Attribute == GetShotgunCurrentAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetShotgunMaxAmmo());
	}
	if (Attribute == GetShotgunCurrentReserveAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetShotgunMaxReserveAmmo());
	}
	
	if (Attribute == GetDecoyGrenadeCurrentChargesAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetDecoyGrenadeMaxCharges());
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
	if (Data.EvaluatedData.Attribute == GetRifleCurrentReserveAmmoAttribute())
	{
		SetRifleCurrentReserveAmmo(FMath::Clamp(GetRifleCurrentReserveAmmo(), 0.f, GetRifleMaxReserveAmmo()));
	}
	
	if (Data.EvaluatedData.Attribute == GetPlasmaGrenadeCurrentChargesAttribute())
	{
		SetPlasmaGrenadeCurrentCharges(FMath::Clamp(GetPlasmaGrenadeCurrentCharges(), 0.f, GetPlasmaGrenadeMaxCharges()));
	}
	
	if (Data.EvaluatedData.Attribute == GetMagnumCurrentAmmoAttribute())
	{
		SetMagnumCurrentAmmo(FMath::Clamp(GetMagnumCurrentAmmo(), 0.f, GetMagnumMaxAmmo()));
	}
	if (Data.EvaluatedData.Attribute == GetMagnumCurrentReserveAmmoAttribute())
	{
		SetMagnumCurrentReserveAmmo(FMath::Clamp(GetMagnumCurrentReserveAmmo(), 0.f, GetMagnumMaxReserveAmmo()));
	}
	
	if (Data.EvaluatedData.Attribute == GetTurretCurrentChargesAttribute())
	{
		SetTurretCurrentCharges(FMath::Clamp(GetTurretCurrentCharges(), 0.f, GetTurretMaxCharges()));
	}
	
	if (Data.EvaluatedData.Attribute == GetShotgunCurrentAmmoAttribute())
	{
		SetShotgunCurrentAmmo(FMath::Clamp(GetShotgunCurrentAmmo(), 0.f, GetShotgunMaxAmmo()));
	}
	if (Data.EvaluatedData.Attribute == GetShotgunCurrentReserveAmmoAttribute())
	{
		SetShotgunCurrentReserveAmmo(FMath::Clamp(GetShotgunCurrentReserveAmmo(), 0.f, GetShotgunMaxReserveAmmo()));
	}
	
	if (Data.EvaluatedData.Attribute == GetDecoyGrenadeCurrentChargesAttribute())
	{
		SetDecoyGrenadeCurrentCharges(FMath::Clamp(GetDecoyGrenadeCurrentCharges(), 0.f, GetDecoyGrenadeMaxCharges()));
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

void UWeaponAttributeSet::OnRep_RifleCurrentReserveAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, RifleCurrentReserveAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_RifleMaxReserveAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, RifleMaxReserveAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_PlasmaGrenadeCurrentCharges(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PlasmaGrenadeCurrentCharges, OldValue);
}

void UWeaponAttributeSet::OnRep_PlasmaGrenadeMaxCharges(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, PlasmaGrenadeMaxCharges, OldValue);
}

void UWeaponAttributeSet::OnRep_MagnumCurrentAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagnumCurrentAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_MagnumMaxAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagnumMaxAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_MagnumCurrentReserveAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagnumCurrentReserveAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_MagnumMaxReserveAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagnumMaxReserveAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_TurretCurrentCharges(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, TurretCurrentCharges, OldValue);
}

void UWeaponAttributeSet::OnRep_TurretMaxCharges(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, TurretMaxCharges, OldValue);
}

void UWeaponAttributeSet::OnRep_ShotgunCurrentAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ShotgunCurrentAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_ShotgunMaxAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ShotgunMaxAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_ShotgunCurrentReserveAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ShotgunCurrentReserveAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_ShotgunMaxReserveAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ShotgunMaxReserveAmmo, OldValue);
}

void UWeaponAttributeSet::OnRep_DecoyGrenadeCurrentCharges(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DecoyGrenadeCurrentCharges, OldValue);
}

void UWeaponAttributeSet::OnRep_DecoyGrenadeMaxCharges(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DecoyGrenadeMaxCharges, OldValue);
}
