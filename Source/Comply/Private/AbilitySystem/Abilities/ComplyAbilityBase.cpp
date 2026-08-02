// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/ComplyAbilityBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/Data/Player/Abilities/ComplyAbilityData.h"


void UComplyAbilityBase::OnMontageCompleted()
{
	PlayActivationMontageTask->EndTask();
}
 
void UComplyAbilityBase::OnMontageCancelled()
{
	PlayActivationMontageTask->EndTask();
}

UGameplayEffect* UComplyAbilityBase::GetCostGameplayEffect() const
{
	if (AbilityData && AbilityData->CostEffect)
	{
		return AbilityData->CostEffect->GetDefaultObject<UGameplayEffect>();
	}
	return Super::GetCostGameplayEffect();
}

UGameplayEffect* UComplyAbilityBase::GetCooldownGameplayEffect() const
{
	if (AbilityData && AbilityData->CooldownEffect)
	{
		return AbilityData->CooldownEffect->GetDefaultObject<UGameplayEffect>();
	}
	return Super::GetCooldownGameplayEffect();
}
