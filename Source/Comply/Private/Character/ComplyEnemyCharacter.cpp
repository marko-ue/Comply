// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyEnemyCharacter.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"


AComplyEnemyCharacter::AComplyEnemyCharacter()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");
	
	// Causes enemies to avoid each other so they don't clip through each other
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceWeight = 0.2f;
}

UAbilitySystemComponent* AComplyEnemyCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

// For enemies, ASC ability actor info values can just be initialized on BeginPlay
// They will have a valid controller when the game starts, as they spawn on the server
void AComplyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(GetAbilitySystemComponent())) return;
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	
	if (!HasAuthority()) return;

	GiveStartupAbilities();
	
	GetAbilitySystemComponent()->SetNumericAttributeBase(UComplyAttributeSet::GetMaxHealthAttribute(), BaseMaxHealth);
	GetAbilitySystemComponent()->SetNumericAttributeBase(UComplyAttributeSet::GetHealthAttribute(), BaseHealth);
	GetAbilitySystemComponent()->SetNumericAttributeBase(UComplyAttributeSet::GetMaxArmorPenetrationAttribute(), BaseMaxArmorPenetration);
	GetAbilitySystemComponent()->SetNumericAttributeBase(UComplyAttributeSet::GetArmorPenetrationAttribute(), BaseArmorPenetration);
}
