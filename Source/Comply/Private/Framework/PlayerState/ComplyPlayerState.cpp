// Copyright © 2026 Marko. All rights reserved.


#include "Framework/PlayerState/ComplyPlayerState.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Character/ComplyPlayerCharacter.h"


AComplyPlayerState::AComplyPlayerState()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");
	WeaponAttributeSet = CreateDefaultSubobject<UWeaponAttributeSet>("WeaponAttributeSet");
	
	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AComplyPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

// Every player name gets the Radmin suffix removed
void AComplyPlayerState::SetPlayerName(const FString& S)
{
	Super::SetPlayerName(TrimRadminSuffix(S));
}

// Trims the suffix added to Radmin usernames
FString AComplyPlayerState::TrimRadminSuffix(const FString& RawName)
{
	int32 LastDashIndex;
	if (RawName.FindLastChar(TEXT('-'), LastDashIndex))
	{
		return RawName.Left(LastDashIndex);
	}
	return RawName;
}
