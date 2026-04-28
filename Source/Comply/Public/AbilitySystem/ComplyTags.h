#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary_Ranger);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Utility_Ranger);
		
		namespace AssetTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ApplyShieldedEffectAbility);
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
	}
	
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceRifleReserveAmmo);
	}
}