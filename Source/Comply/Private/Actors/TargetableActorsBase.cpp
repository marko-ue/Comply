// Copyright © 2026 Marko. All rights reserved.


#include "Actors/TargetableActorsBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"


ATargetableActorsBase::ATargetableActorsBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");
}

void ATargetableActorsBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority() && GetAbilitySystemComponent() && InitializeAttributesEffect)
	{
		GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
		
		FGameplayEffectContextHandle AttributesContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
		FGameplayEffectSpecHandle AttributesSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, AttributesContextHandle);
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*AttributesSpecHandle.Data.Get());	
	}
}

void ATargetableActorsBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UAbilitySystemComponent* ATargetableActorsBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void ATargetableActorsBase::TakeDamage_Implementation()
{
	FGameplayCueParameters CueParams;
	CueParams.Location = GetActorLocation();
	GetAbilitySystemComponent()->ExecuteGameplayCue(ComplyTags::GameplayCues::TargetableActorTakeDamage, CueParams);
}

void ATargetableActorsBase::Die_Implementation()
{
	FGameplayCueParameters CueParams;
	CueParams.Location = GetActorLocation();
	GetAbilitySystemComponent()->ExecuteGameplayCue(ComplyTags::GameplayCues::TargetableActorDeath, CueParams);
	
	bWasKilledByEnemies = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Death cue"));
	
	Destroy();
}
