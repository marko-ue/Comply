// Copyright © 2026 Marko. All rights reserved.


#include "Framework/PlayerState/ComplyPlayerState.h"

#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyAttributeSet.h"

AComplyPlayerState::AComplyPlayerState()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");
	
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AComplyPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
