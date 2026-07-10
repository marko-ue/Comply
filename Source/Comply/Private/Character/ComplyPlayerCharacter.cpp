// Copyright © 2026 Marko. All rights reserved.

#include "Character/ComplyPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "CableComponent.h"
#include "Comply.h"
#include "ComplyPlayerController.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "Interface/Player/InteractableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	DOREPLIFETIME(AComplyPlayerCharacter, bIsDowned);
}

void AComplyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Handles the case where SetupPlayerInputComponent fires after OnRep_PlayerState and ASC actor info hasn't been initialized yet
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->InitAbilityActorInfo(GetPlayerState(), this);
	}
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ThisClass::PrimaryActionPressed);
		EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &ThisClass::PrimaryActionReleased);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Started, this, &ThisClass::SecondaryActionPressed);
		EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &ThisClass::SecondaryActionReleased);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ThisClass::SprintActionPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ThisClass::SprintActionReleased);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::InteractActionPressed);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ThisClass::InteractActionReleased);
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
	
	GetAbilitySystemComponent()->SetNumericAttributeBase(UComplyAttributeSet::GetMaxArmorAttribute(), BaseMaxArmor);
	GetAbilitySystemComponent()->SetNumericAttributeBase(UComplyAttributeSet::GetArmorAttribute(), BaseArmor);
	
	// Only give startup abilities if not in the lobby map
	FString MapName = GetWorld()->GetMapName();
	if (!MapName.Contains("Lobby"))
	{
		GiveStartupAbilities();
	}
	
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
	
	if (IsLocallyControlled()) { TraceForInteractable(); }
}

UAbilitySystemComponent* AComplyPlayerCharacter::GetAbilitySystemComponent() const
{
	AComplyPlayerState* ComplyPlayerState = Cast<AComplyPlayerState>(GetPlayerState());
	if (!IsValid(ComplyPlayerState)) return nullptr;
	
	return ComplyPlayerState->GetAbilitySystemComponent();
}

URangedWeaponAbilityBase* AComplyPlayerCharacter::GetEquippedPrimaryWeapon() const
{
	if (!GetAbilitySystemComponent()) return nullptr;
	
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == EquippedPrimaryWeaponClass)
		{
			return Cast<URangedWeaponAbilityBase>(Spec.GetPrimaryInstance() ? Spec.GetPrimaryInstance() : Spec.Ability.Get());
		}
	}
	return nullptr;
}

void AComplyPlayerCharacter::DownPlayer()
{
	if (!GetAbilitySystemComponent()) return;
	
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_Downed)) return;
    
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DownedEffectClass, 1.f, ContextHandle);
	ActiveDownedEffectHandle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	// Trigger OnRep on both client and server, it handles ragdolling the player and also reviving them
	bIsDowned = true;
	OnRep_IsDowned();
}

void AComplyPlayerCharacter::OnRep_IsDowned()
{
	if (bIsDowned)
	{
		// Ragdoll player
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Interact, ECR_Block);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);
		GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
		GetMesh()->SetSimulatePhysics(true);
		
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		RevivePlayer();
	}
}

void AComplyPlayerCharacter::RevivePlayer()
{
	// Trace down from capsule to find ground to spawn the actor there
	FHitResult GroundHit;
	FVector Start = GetCapsuleComponent()->GetComponentLocation();
	FVector End = Start - FVector(0.f, 0.f, 500.f);
	GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility);
    
	if (GroundHit.bBlockingHit)
	{
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		SetActorLocation(GroundHit.ImpactPoint + FVector(0.f, 0.f, CapsuleHalfHeight));
	}
	
	// Restore relevant attributes to defaults before death
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ReviveEffectClass, 1.f, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	// Restore to settings from before ragdoll
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetWorldLocation(GetCapsuleComponent()->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_None);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Player, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Interact, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
	
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	
	if (ActiveDownedEffectHandle.IsValid() && GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->RemoveActiveGameplayEffect(ActiveDownedEffectHandle);
	}
}

// Server RPC called from the revive ability
void AComplyPlayerCharacter::Server_ReviveTarget_Implementation(AComplyPlayerCharacter* Target)
{
	if (!Target) return;
	Target->bIsDowned = false; // Variable handles replicating to client via OnRep
	Target->RevivePlayer(); // Runs the function on the server
}

void AComplyPlayerCharacter::Server_FaceTarget_Implementation(ACharacter* Target)
{
	if (!Target) return;
	bUseControllerRotationYaw = false;
	FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), Target->GetActorLocation());
	SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
}

void AComplyPlayerCharacter::SetEquippedPrimaryWeapon(TSubclassOf<URangedWeaponAbilityBase> NewWeaponClass)
{
	EquippedPrimaryWeaponClass = NewWeaponClass;
}

void AComplyPlayerCharacter::PrimaryActionPressed()
{
	if (!GetAbilitySystemComponent()) return;
	
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
	
	if (ApplyFireEffectAbilityClass)
	{
		if (!GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_FiringBlocked))
		{
			GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyFireEffectAbilityClass);
		}
	}

	bFireInputHeld = true;
}

void AComplyPlayerCharacter::PrimaryActionReleased()
{
	if (!GetAbilitySystemComponent()) return;
	
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
	if (GetAbilitySystemComponent())
	{
		if (ApplyAimEffectAbilityClass)
		{
			GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyAimEffectAbilityClass);
		}
	}
}

void AComplyPlayerCharacter::SecondaryActionReleased()
{
	if (!GetAbilitySystemComponent()) return;
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

void AComplyPlayerCharacter::InteractActionPressed()
{
	if (!GetAbilitySystemComponent()) return;
	
	// Call the interact function on the current focused interactable, passing in the player's controller
	if (CurrentFocusedInteractable) { CurrentFocusedInteractable->Interact(GetController<APlayerController>()); }
	
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Interact))
		{
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
		}
	}
}

void AComplyPlayerCharacter::InteractActionReleased()
{
	if (!GetAbilitySystemComponent()) return;
	
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		// For interact abilities that require holding (like reviving the player)
		// The ability will be canceled if the interact action is released, which makes it not go through with the revive if input released
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Interact))
		{
			GetAbilitySystemComponent()->CancelAbilityHandle(Spec.Handle);
		}
	}
}

void AComplyPlayerCharacter::SprintActionPressed()
{
	if (GetCharacterMovement()->IsFalling()) return;
	
	if (GetAbilitySystemComponent())
	{
		FGameplayTagContainer Tag;
		Tag.AddTag(ComplyTags::ComplyAbilities::Sprint);
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(Tag);
	}
}

void AComplyPlayerCharacter::SprintActionReleased()
{
	if (!GetAbilitySystemComponent()) return;
	
	// Predictively set max walk speed back to the base value on the client, since GE removal is not replicated
	if (GetCharacterMovement() && IsLocallyControlled() && !HasAuthority())
	{
		int32 TotemStacks = GetAbilitySystemComponent()->GetTagCount(ComplyTags::States::State_TotemBuffed);
		float CorrectSpeed = 500.f + (TotemStacks * TotemSpeedBonusPerStack);
		GetCharacterMovement()->MaxWalkSpeed = CorrectSpeed;
	}

	FGameplayTagContainer Tag;
	Tag.AddTag(ComplyTags::ComplyAbilities::Sprint);
	GetAbilitySystemComponent()->CancelAbilities(&Tag);
}

void AComplyPlayerCharacter::ReloadActionPressed()
{
	if (!GetAbilitySystemComponent()) return;
	
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(ComplyTags::ComplyAbilities::InputTags::Input_Reload))
		{
			// Play the prepare animation locally so a delay at high ping is not felt if player can reload
			if (IsLocallyControlled() && !HasAuthority())
			{
				if (URangedWeaponAbilityBase* Weapon = GetEquippedPrimaryWeapon())
				{
					bool bFound = false;
					const float CurrentAmmo = GetAbilitySystemComponent()->GetGameplayAttributeValue(Weapon->GetCurrentAmmoAttribute(), bFound);
					const float MaxAmmo = GetAbilitySystemComponent()->GetGameplayAttributeValue(Weapon->GetMaxAmmoAttribute(), bFound);
					const float ReserveAmmo = GetAbilitySystemComponent()->GetGameplayAttributeValue(Weapon->GetCurrentReserveAmmoAttribute(), bFound);
                    
					if (!FMath::IsNearlyEqual(CurrentAmmo, MaxAmmo) && ReserveAmmo > 0.f &&
						!GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_Firing))
					{
						PlayAnimMontage(PrepareReloadMontage);
					}
				}
			}
			
			GetAbilitySystemComponent()->TryActivateAbility(Spec.Handle);
			break;
		}
	}
}

// If the ability is active and this input is pressed, cancel the input. This will properly cancel the ability client side and notify the server
void AComplyPlayerCharacter::CancelPreviewActionPressed()
{
	if (!GetAbilitySystemComponent()) return;
	
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
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->TryActivateAbilityByClass(EquipPrimaryAbilityClass);
	}
}

void AComplyPlayerCharacter::EquipUtilityActionPressed()
{
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->TryActivateAbilityByClass(EquipUtilityAbilityClass);
	}
}

void AComplyPlayerCharacter::EquipThrowableActionPressed()
{
	if (GetAbilitySystemComponent())
	{
		GetAbilitySystemComponent()->TryActivateAbilityByClass(EquipThrowableAbilityClass);
	}
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

// Traces to the camera each tick, checking for interactable actors
void AComplyPlayerCharacter::TraceForInteractable()
{
	FVector2D ViewportSize = FVector2D();
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(
		this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		
		float DistanceToCharacter = (GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100);
		
		FVector End = Start + CrosshairWorldDirection * 200;
		
		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

		AActor* HitActor = bHit ? Hit.GetActor() : nullptr;
		IInteractableInterface* Interactable = HitActor ? Cast<IInteractableInterface>(HitActor) : nullptr;

		if (Interactable != CurrentFocusedInteractable)
		{
			if (CurrentFocusedInteractable) CurrentFocusedInteractable->HideInteractionPrompt();

			CurrentFocusedInteractable = Interactable;

			if (CurrentFocusedInteractable) CurrentFocusedInteractable->ShowInteractionPrompt();
		}
	}
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

// When speed is buffed, the movement speed attribute is changed, and the character's new movement speed is set here
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
