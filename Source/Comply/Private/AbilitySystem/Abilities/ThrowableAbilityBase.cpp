// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"

void UThrowableAbilityBase::ConfirmThrow()
{
}

void UThrowableAbilityBase::SpawnPreview()
{
}

void UThrowableAbilityBase::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
}

void UThrowableAbilityBase::CancelThrow()
{
}

void UThrowableAbilityBase::SafeRemoveThrowingTag() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	// Only remove if we actually own a count of this tag
	if (ASC->GetTagCount(ComplyTags::States::State_ThrowableThrowing) > 0)
	{
		ASC->RemoveLooseGameplayTag(ComplyTags::States::State_ThrowableThrowing);
	}
}
