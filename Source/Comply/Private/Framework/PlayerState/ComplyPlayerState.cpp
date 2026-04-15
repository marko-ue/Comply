// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PlayerState/ComplyPlayerState.h"

#include "AbilitySystem/ComplyAbilitySystemComponent.h"

AComplyPlayerState::AComplyPlayerState()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AComplyPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
