// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ComplyCharacterBase.h"

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

// Called to bind functionality to input
void AComplyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

