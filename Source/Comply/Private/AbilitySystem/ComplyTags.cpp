#include "AbilitySystem/ComplyTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary_Ranger, "ComplyTags.Abilities.Ranger.Primary", "Primary ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Utility_Ranger, "ComplyTags.Abilities.Ranger.Utility", "Utility ability asset tag")
		
		namespace AssetTags
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ApplyShieldedEffectAbility, "ComplyTags.Abilities.Ranger.ApplyShieldedEffectAbility", "Apply shielded effect ability asset tag")
		}
		
		namespace InputTags
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Primary, "ComplyTags.Abilities.InputTags.Input_Primary", "Input tag for primary abilities")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_OneShotUtility, "ComplyTags.Abilities.InputTags.Input_OneShotUtility", "Input tag for utilities that are used as soon as their input is pressed")
		}
		
		namespace DamageTypes
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "ComplyTags.Abilities.DamageTypes.Physical", "Physical damage type")
		}
	}
	
	namespace States
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Aiming, "ComplyTags.States.Aiming", "Is player aiming")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Firing, "ComplyTags.States.Firing", "Is player firing")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Shielded, "ComplyTags.States.Shielded", "Is player being shielded")
	}
}