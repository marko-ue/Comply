// Copyright © 2026 Marko. All rights reserved.

#include "Character/ComplyPlayerCharacter.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
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
	GiveStartupAbilities();
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
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &AComplyPlayerCharacter::PrimaryActionPressed);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &AComplyPlayerCharacter::SecondaryActionPressed);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &AComplyPlayerCharacter::SecondaryActionReleased);
	}
}

void AComplyPlayerCharacter::PrimaryActionPressed()
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

void AComplyPlayerCharacter::SecondaryActionPressed()
{
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(AimingEffectClass, 1.f, ContextHandle);
	ActiveAimingEffectHandle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AComplyPlayerCharacter::SecondaryActionReleased()
{
	GetAbilitySystemComponent()->RemoveActiveGameplayEffect(ActiveAimingEffectHandle);
}



