// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/ComplyAbilityBase.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"


void UComplyAbilityBase::OnMontageCompleted()
{
	PlayMontageTask->EndTask();
}

void UComplyAbilityBase::OnMontageCancelled()
{
	PlayMontageTask->EndTask();
}