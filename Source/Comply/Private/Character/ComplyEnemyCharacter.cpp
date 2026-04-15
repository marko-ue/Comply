// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ComplyEnemyCharacter.h"

#include "AbilitySystem/ComplyAbilitySystemComponent.h"

AComplyEnemyCharacter::AComplyEnemyCharacter()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
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
}
