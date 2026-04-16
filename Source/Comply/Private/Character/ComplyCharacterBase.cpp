// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystem/ComplyTags.h"

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
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AComplyCharacterBase::GiveStartupAbilities()
{
	if (!GetAbilitySystemComponent()) return;
	
	for (const FAbilitySet& Set: StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Set.AbilityClass);
		AbilitySpec.DynamicAbilityTags.AddTag(Set.InputTag);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("Ability given"));
	}
}

void AComplyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &AComplyCharacterBase::PrimaryActionPressed);
	}
}

void AComplyCharacterBase::PrimaryActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Primary))
		{
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			break;
		}
	}
}



