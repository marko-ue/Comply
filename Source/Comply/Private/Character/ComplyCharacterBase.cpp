// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

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

void AComplyCharacterBase::InitializeAttributes() const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set"));
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

// Called every frame
void AComplyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AComplyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

