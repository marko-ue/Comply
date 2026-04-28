// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Character/ComplyPlayerCharacter.h"

// Sets default values
AComplyCharacterBase::AComplyCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

UAbilitySystemComponent* AComplyCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

// Called when the game starts or when spawned
void AComplyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}



// Called every frame
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

void AComplyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}





