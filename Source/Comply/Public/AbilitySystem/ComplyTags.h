#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		
		namespace InputTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Primary);
		}
		
		namespace DamageTypes
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical);
		}
	}
}