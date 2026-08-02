// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyEnemyCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "AbilitySystem/Data/Enemy/Stats/ComplyEnemyCharacterStatData.h"
#include "GameFramework/CharacterMovementComponent.h"


AComplyEnemyCharacter::AComplyEnemyCharacter()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");
	
	// Causes enemies to avoid each other so they don't clip through each other
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceWeight = 0.05f;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 500.f;
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
	
	InitializeAttributes();

	GiveStartupAbilities();
}

void AComplyEnemyCharacter::InitializeAttributes() const
{
	if (!EnemyStatData) return;

	checkf(EnemyStatData, TEXT("EnemyStatData not set on %s"), *GetName());

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EnemyStatData->InitializeAttributesEffect, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ComplyTags::SetByCaller::Stats::SBC_MaxHealth, EnemyStatData->MaxHealth);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ComplyTags::SetByCaller::Stats::SBC_MaxArmorPenetration, EnemyStatData->ArmorPenetration);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}
