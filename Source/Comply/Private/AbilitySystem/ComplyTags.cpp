#include "AbilitySystem/ComplyTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ComplyTags.Abilities.Primary", "Primary ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Utility, "ComplyTags.Abilities.Utility", "Utility ability asset tag")
		
		namespace InputTags
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Primary, "ComplyTags.Abilities.InputTags.Input_Primary", "Input tag for primary abilities")
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
	}
}