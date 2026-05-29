#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Utility);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Throwable);
		
		namespace AssetTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ApplyShieldedEffectAbility);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ApplyConfusedEffectAbility);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip_Primary)
		}
		
		namespace InputTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_OneShotUtility);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Reload);
		}
		
		namespace DamageTypes
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical);
		}
	}
	
	namespace States
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Aiming);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Firing);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Shielded)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Reloading);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Confused)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Distracted)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equipping)
	}
	
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceRifleReserveAmmo);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceMagnumReserveAmmo);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceShotgunReserveAmmo);
	}
}