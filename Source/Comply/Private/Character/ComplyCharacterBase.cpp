// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "GameplayEffect.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/CapsuleComponent.h"


AComplyCharacterBase::AComplyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* AComplyCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AComplyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AComplyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AComplyCharacterBase::InitializeAttributes() const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set"));
	checkf(IsValid(InitializeWeaponAttributesEffect), TEXT("InitializeWeaponAttributesEffect not set"));
	
	FGameplayEffectContextHandle AttributesContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle AttributesSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, AttributesContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*AttributesSpecHandle.Data.Get());
	
	FGameplayEffectContextHandle WeaponAttributesContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle WeaponAttributesSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeWeaponAttributesEffect, 1.f, WeaponAttributesContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*WeaponAttributesSpecHandle.Data.Get());
}

void AComplyCharacterBase::GiveStartupAbilities()
{
	if (!GetAbilitySystemComponent()) return;
	
	for (const FAbilitySet& Set: StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Set.AbilityClass);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Set.InputTag);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Set.AbilityTypeTag);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		// Store equipped primary weapon class
		if (Set.AbilityClass->IsChildOf(URangedWeaponAbilityBase::StaticClass()))
		{
			if (AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(this))
			{
				PlayerCharacter->EquippedPrimaryWeaponClass = Set.AbilityClass;
			}
		}
	}
}

void AComplyCharacterBase::ClearStartupAbilities()
{
	if (!GetAbilitySystemComponent()) return;

	for (const FAbilitySet& Set : StartupAbilities)
	{
		FGameplayAbilitySpec* AbilitySpec = GetAbilitySystemComponent()->FindAbilitySpecFromClass(Set.AbilityClass);
		if (AbilitySpec)
		{
			GetAbilitySystemComponent()->ClearAbility(AbilitySpec->Handle);
		}
	}
}

void AComplyCharacterBase::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
    
	if (AController* C = GetController())
	{
		C->UnPossess();
	}

	SetLifeSpan(3.f);
}

void AComplyCharacterBase::ActivateInitialAbility() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	// Reset all block counts first
	FGameplayTagContainer AllWeaponTags;
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Primary);
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Utility);
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Throwable);
	ASC->UnBlockAbilitiesWithTags(AllWeaponTags);

	// Cancel everything, then block all except this ability
	ASC->CancelAbilities(&AllWeaponTags, nullptr);

	// Unblock the relevant ability related to this equip ability
	FGameplayTagContainer TagsToBlock = AllWeaponTags;
	TagsToBlock.RemoveTag(ComplyTags::ComplyAbilities::Primary);
	ASC->BlockAbilitiesWithTags(TagsToBlock);
}

void AComplyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
