// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/ComplyAbilityBase.h"

void UComplyAbilityBase::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UComplyAbilityBase::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
