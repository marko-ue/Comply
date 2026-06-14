// Copyright © 2026 Marko. All rights reserved.

#include "Character/ComplyPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "CableComponent.h"
#include "ComplyPlayerController.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
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
	
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("weapon_r"));
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AComplyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AComplyPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AComplyPlayerCharacter, EquippedPrimaryWeaponClass);
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
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ThisClass::ReloadActionPressed);
		EnhancedInputComponent->BindAction(CancelPreviewAction, ETriggerEvent::Started, this, &ThisClass::CancelPreviewActionPressed);
		EnhancedInputComponent->BindAction(EquipPrimaryAction, ETriggerEvent::Started, this, &ThisClass::EquipPrimaryActionPressed);
		EnhancedInputComponent->BindAction(EquipUtilityAction, ETriggerEvent::Started, this, &ThisClass::EquipUtilityActionPressed);
		EnhancedInputComponent->BindAction(EquipThrowableAction, ETriggerEvent::Started, this, &ThisClass::EquipThrowableActionPressed);
	}
}

// For the server, ASC ability actor info is initialized here
// At this point, we know the character has been possessed by a controller (not too early to initialize)
void AComplyPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!GetAbilitySystemComponent() || !HasAuthority()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	
	WeaponMesh->SetStaticMesh(PrimaryMesh);
	
	InitializeAttributes();
	GiveStartupAbilities();
	ActivateInitialAbility();
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_Aiming,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComplyPlayerCharacter::OnAimingTagChanged);
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_Distracted,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComplyPlayerCharacter::OnDistractedTagChanged);
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_TotemBuffed,
		EGameplayTagEventType::AnyCountChange).AddUObject(this, &AComplyPlayerCharacter::OnTotemBuffedTagChanged);
	
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
	UComplyAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &AComplyPlayerCharacter::OnMovementSpeedAttributeChanged);
}

// For clients, ASC ability actor info is initialized here
// We know the player state is valid as it has replicated
void AComplyPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	
	WeaponMesh->SetStaticMesh(PrimaryMesh);
	
	ActivateInitialAbility();
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_Aiming,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComplyPlayerCharacter::OnAimingTagChanged);
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_Distracted,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AComplyPlayerCharacter::OnDistractedTagChanged);
	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(ComplyTags::States::State_TotemBuffed,
		EGameplayTagEventType::AnyCountChange).AddUObject(this, &AComplyPlayerCharacter::OnTotemBuffedTagChanged);
	
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(
	UComplyAttributeSet::GetMovementSpeedAttribute()).AddUObject(this, &AComplyPlayerCharacter::OnMovementSpeedAttributeChanged);
}

void AComplyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	bIsAiming ? ZoomIn(DeltaTime) : ZoomOut(DeltaTime);
	
	// Handles how the character should rotate depending on if the player is aiming and/or firing
	UpdateRotationMode(DeltaTime);
}

UAbilitySystemComponent* AComplyPlayerCharacter::GetAbilitySystemComponent() const
{
	AComplyPlayerState* ComplyPlayerState = Cast<AComplyPlayerState>(GetPlayerState());
	if (!IsValid(ComplyPlayerState)) return nullptr;
	
	return ComplyPlayerState->GetAbilitySystemComponent();
}

URangedWeaponAbilityBase* AComplyPlayerCharacter::GetEquippedPrimaryWeapon() const
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == EquippedPrimaryWeaponClass)
		{
			return Cast<URangedWeaponAbilityBase>(Spec.GetPrimaryInstance() ? Spec.GetPrimaryInstance() : Spec.Ability.Get());
		}
	}
	return nullptr;
}

void AComplyPlayerCharacter::SetEquippedPrimaryWeapon(TSubclassOf<URangedWeaponAbilityBase> NewWeaponClass)
{
	EquippedPrimaryWeaponClass = NewWeaponClass;
}

void AComplyPlayerCharacter::PrimaryActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::Throwable))
		{
			// If the ability is already active, confirm the input
			// Confirming the input lets the ability continue with its functionality, the preview will now be removed
			if (Spec.IsActive())
			{
				GetAbilitySystemComponent()->LocalInputConfirm();
				break;
			}
			else
			{
				GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
				break;
			}
		}
		
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::Utility))
		{
			// If the ability is already active, confirm the input
			// Confirming the input lets the ability continue with its functionality, the preview will now be removed
			if (Spec.IsActive())
			{
				GetAbilitySystemComponent()->LocalInputConfirm();
				break;
			}
		}
		
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Primary))
		{
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			bIsFiring = true;
		}
	}

	GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyFireEffectAbilityClass);
	bFireInputHeld = true;
}

void AComplyPlayerCharacter::PrimaryActionReleased()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		// Confirm the throw of the throwable once the primary input is released, removing the preview path
		// Only for throwable abilities that use confirm input on release
		if (Spec.Ability->GetAssetTags().HasTagExact(ComplyTags::ComplyAbilities::Throwable)
			&& Spec.IsActive() && Cast<UThrowableAbilityBase>(Spec.GetPrimaryInstance())->bConfirmOnRelease == true) 
		{
			GetAbilitySystemComponent()->LocalInputConfirm();
			break;
		}
	}

	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == ApplyFireEffectAbilityClass)
		{
			GetAbilitySystemComponent()->CancelAbility(Spec.Ability);
			bIsFiring = false;
			break;
		}
	}

	bFireInputHeld = false;
}

void AComplyPlayerCharacter::SecondaryActionPressed()
{
	bIsAiming = true;
	GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyAimEffectAbilityClass);
}

void AComplyPlayerCharacter::SecondaryActionReleased()
{
	bIsAiming = false;
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == ApplyAimEffectAbilityClass)
		{
			GetAbilitySystemComponent()->CancelAbility(Spec.Ability);
			break;
		}
	}
}

void AComplyPlayerCharacter::ReloadActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Reload))
		{
			// Play the prepare animation locally so a delay at high ping is not felt
			if (IsLocallyControlled() && !HasAuthority())
			{
				PlayAnimMontage(PrepareReloadMontage);
			}
			
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			break;
		}
	}
}

// If the ability is active and this input is pressed, cancel the input. This will properly cancel the ability client side and notify the server
void AComplyPlayerCharacter::CancelPreviewActionPressed()
{
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Primary))
		{
			if (Spec.IsActive())
			{
				GetAbilitySystemComponent()->LocalInputCancel();
			}
			break;
		}
	}
}

void AComplyPlayerCharacter::EquipPrimaryActionPressed()
{
	GetAbilitySystemComponent()->TryActivateAbilityByClass(EquipPrimaryAbilityClass);
}

void AComplyPlayerCharacter::EquipUtilityActionPressed()
{
	GetAbilitySystemComponent()->TryActivateAbilityByClass(EquipUtilityAbilityClass);
}

void AComplyPlayerCharacter::EquipThrowableActionPressed()
{
	GetAbilitySystemComponent()->TryActivateAbilityByClass(EquipThrowableAbilityClass);
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

void AComplyPlayerCharacter::OnAimingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount > 0;
}

void AComplyPlayerCharacter::UpdateRotationMode(float DeltaTime)
{
	if (!GetAbilitySystemComponent()) return;
	
	// With this check, the server computes rotation and replicates it
	// The owning client computes rotation locally for responsiveness
	// Simulated proxies display the replicated rotation instead of fighting with local interpolation
	if (!IsLocallyControlled() && !HasAuthority()) return;

	const bool bDoesFiringTagExist = GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_Firing);
	const bool bShouldControlRotation = bIsAiming || bDoesFiringTagExist;

	FRotator TargetRotation;

	if (bShouldControlRotation)
	{
		TargetRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	}
	else
	{
		const FVector Velocity = GetCharacterMovement()->Velocity;
		if (Velocity.IsNearlyZero()) return;
		TargetRotation = FRotator(0.f, Velocity.Rotation().Yaw, 0.f);
	}

	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 25.f);
	SetActorRotation(NewRotation);
}

// If friendly fire is on and the distracted tag was applied by the decoy grenade, apply flashbang effect to affected players
void AComplyPlayerCharacter::OnDistractedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount <= 0) return;
	if (!IsLocallyControlled()) return;
	
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS && GS->bFriendlyFire)
	{
		AComplyPlayerController* PC = Cast<AComplyPlayerController>(GetController());
		if (PC) PC->ShowFlashbangEffect();
	}
}

// This callback is used to buff player speed. Damage buffs are handled in the ranged weapon ability base.
void AComplyPlayerCharacter::OnTotemBuffedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (ActiveTotemSpeedBuffEffectHandle.IsValid())
	{
		GetAbilitySystemComponent()->RemoveActiveGameplayEffect(ActiveTotemSpeedBuffEffectHandle);
	}

	if (NewCount <= 0) return;

	// Apply the movement speed buff, stacking with the amount of buffs
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(TotemSpeedBuffEffectClass, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, 
		ComplyTags::SetByCaller::SBC_TotemSpeedBuff, TotemSpeedBonusPerStack * NewCount);
	ActiveTotemSpeedBuffEffectHandle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

// When the totem buffs speed, the movement speed attribute is changed, and the character's new movement speed is set here
void AComplyPlayerCharacter::OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

FVector AComplyPlayerCharacter::GetScaleForSlot(EWeaponSlot Slot)
{
	switch (Slot)
	{
		case EWeaponSlot::None:		 return FVector::ZeroVector;
		case EWeaponSlot::Primary:   return PrimaryMeshScale;
		case EWeaponSlot::Utility:   return UtilityMeshScale;
		case EWeaponSlot::Throwable: return ThrowableMeshScale;
	}
	return FVector::ZeroVector;
}

UStaticMesh* AComplyPlayerCharacter::GetMeshForSlot(EWeaponSlot Slot)
{
	switch (Slot)
	{
		case EWeaponSlot::None:		 return nullptr;
		case EWeaponSlot::Primary:   return PrimaryMesh;
		case EWeaponSlot::Utility:   return UtilityMesh;
		case EWeaponSlot::Throwable: return ThrowableMesh;
	}
	return nullptr;
}

void AComplyPlayerCharacter::OnWeaponEquipped(EWeaponSlot Slot)
{
	if (Slot == CurrentEquippedSlot) return; // Already holding this weapon, return so the equip animation is not played again

	CurrentEquippedSlot = Slot;
	
	// Set tag to exactly 1 stack regardless of how many were added previously
	// This is to prevent multiple equipping state tags from stacking when spamming equips
	GetAbilitySystemComponent()->SetLooseGameplayTagCount(ComplyTags::States::State_Equipping, 1);

	UAnimMontage* Montage = nullptr;
	switch (Slot)
	{
		case EWeaponSlot::Primary:   Montage = PrimaryEquipMontage;   break;
		case EWeaponSlot::Utility:   Montage = UtilityEquipMontage;   break;
		case EWeaponSlot::Throwable: Montage = ThrowableEquipMontage; break;
		default: break;
	}

	if (Montage) PlayAnimMontage(Montage);
}

// OnRep handles playing the equip animation for clients
void AComplyPlayerCharacter::OnRep_CurrentEquippedSlot()
{
	UAnimMontage* Montage = nullptr;
	switch (CurrentEquippedSlot)
	{
		case EWeaponSlot::None:		 Montage = nullptr;
		case EWeaponSlot::Primary:   Montage = PrimaryEquipMontage;   break;
		case EWeaponSlot::Utility:   Montage = UtilityEquipMontage;   break;
		case EWeaponSlot::Throwable: Montage = ThrowableEquipMontage; break;
	}
	if (Montage) PlayAnimMontage(Montage);
}

// Called in player ABP when the anim notify broadcasts
void AComplyPlayerCharacter::OnWeaponDrawn()
{
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(ComplyTags::States::State_Equipping);
	WeaponMesh->SetWorldScale3D(GetScaleForSlot(CurrentEquippedSlot));
	WeaponMesh->SetStaticMesh(GetMeshForSlot(CurrentEquippedSlot));
	WeaponMesh->SetVisibility(true);
}

void AComplyPlayerCharacter::ClearEquippedWeapon()
{
	CurrentEquippedSlot = EWeaponSlot::None;
	WeaponMesh->SetVisibility(false);
}
