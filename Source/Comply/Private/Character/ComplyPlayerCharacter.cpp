// Copyright © 2026 Marko. All rights reserved.

#include "Character/ComplyPlayerCharacter.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

AComplyPlayerCharacter::AComplyPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	
}

UAbilitySystemComponent* AComplyPlayerCharacter::GetAbilitySystemComponent() const
{
	AComplyPlayerState* ComplyPlayerState = Cast<AComplyPlayerState>(GetPlayerState());
	if (!IsValid(ComplyPlayerState)) return nullptr;
	
	return ComplyPlayerState->GetAbilitySystemComponent();
}

// For the server, ASC ability actor info is initialized here
// At this point, we know the character has been possessed by a controller (not too early to initialize)
void AComplyPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!GetAbilitySystemComponent() || !HasAuthority()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	InitializeAttributes();
}

// For clients, ASC ability actor info is initialized here
// We know the player state is valid as it has replicated
void AComplyPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
}

void AComplyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AComplyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}



