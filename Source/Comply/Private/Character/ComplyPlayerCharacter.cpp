// Copyright © 2026 Marko. All rights reserved.

#include "Character/ComplyPlayerCharacter.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "AbilitySystem/Abilities/ComplyAbilityBase.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

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

void AComplyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	bIsAiming ? ZoomIn(DeltaTime) : ZoomOut(DeltaTime);
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
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_Aiming,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComplyPlayerCharacter::OnAimingTagChanged);
}

// For clients, ASC ability actor info is initialized here
// We know the player state is valid as it has replicated
void AComplyPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_Aiming,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComplyPlayerCharacter::OnAimingTagChanged);
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
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ThisClass::PrimaryActionPressed);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &ThisClass::PrimaryActionReleased);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::SecondaryActionPressed);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &ThisClass::SecondaryActionReleased);
		EnhancedInputComponent->BindAction(UseUtilityAction, ETriggerEvent::Started, this, &ThisClass::UseUtilityActionPressed);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ThisClass::ReloadActionPressed);
	}
}

void AComplyPlayerCharacter::PrimaryActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Primary))
		{
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			break;
		}
	}

	GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyFireEffectAbilityClass);
	
	bFireInputHeld = true;
}

void AComplyPlayerCharacter::PrimaryActionReleased()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == ApplyFireEffectAbilityClass)
		{
			GetAbilitySystemComponent()->CancelAbility(Spec.Ability);
			break;
		}
	}
	
	bFireInputHeld = false;
}

void AComplyPlayerCharacter::SecondaryActionPressed()
{
	GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyAimEffectAbilityClass);
}

void AComplyPlayerCharacter::SecondaryActionReleased()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == ApplyAimEffectAbilityClass)
		{
			GetAbilitySystemComponent()->CancelAbility(Spec.Ability);
			break;
		}
	}
}

void AComplyPlayerCharacter::UseUtilityActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_OneShotUtility))
		{
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			break;
		}
	}
}

void AComplyPlayerCharacter::ReloadActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		
	}
}

void AComplyPlayerCharacter::OnAimingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount > 0;
}

void AComplyPlayerCharacter::ZoomIn(float DeltaTime)
{
	if (!IsLocallyControlled()) return;
	UCameraComponent* CameraComp = FindComponentByClass<UCameraComponent>();
	CameraComp->FieldOfView = FMath::FInterpTo(
		CameraComp->FieldOfView, AimFOV, DeltaTime, ZoomSpeed);
}

void AComplyPlayerCharacter::ZoomOut(float DeltaTime)
{
	if (!IsLocallyControlled()) return;
	UCameraComponent* CameraComp = FindComponentByClass<UCameraComponent>();
	CameraComp->FieldOfView = FMath::FInterpTo(
		CameraComp->FieldOfView, DefaultFOV, DeltaTime, ZoomSpeed);
}



