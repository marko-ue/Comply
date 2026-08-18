// Copyright © 2026 Marko. All rights reserved.

#include "Character/ComplyPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "ComplyPlayerController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/ComplyUtilityData.h"
#include "AbilitySystem/Data/Player/Grenades/ComplyGrenadeData.h"
#include "AbilitySystem/Data/Player/Input/ComplyInputData.h"
#include "AbilitySystem/Data/Player/Stats/DisruptorData.h"
#include "AbilitySystem/Data/Player/Stats/EnforcerData.h"
#include "AbilitySystem/Data/Player/Stats/RangerData.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Player/InteractableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widgets/ComplyCrosshairWidget.h"
#include "UI/Widgets/ComplyHUDWidget.h"
#include "UI/Widgets/DamageNumbers/DamageNumbersWidget.h"


AComplyPlayerCharacter::AComplyPlayerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
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

	BaseSocketOffsetX = CameraBoom->SocketOffset.X;
}

void AComplyPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AComplyPlayerCharacter, EquippedPrimaryWeaponClass);
	DOREPLIFETIME(AComplyPlayerCharacter, EquippedThrowableClass);
	DOREPLIFETIME(AComplyPlayerCharacter, bIsDowned);}

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
		EnhancedInputComponent->BindAction(InputData->PrimaryAction, ETriggerEvent::Started, this, &ThisClass::PrimaryActionPressed);
		EnhancedInputComponent->BindAction(InputData->PrimaryAction, ETriggerEvent::Completed, this, &ThisClass::PrimaryActionReleased);
		EnhancedInputComponent->BindAction(InputData->SecondaryAction, ETriggerEvent::Started, this, &ThisClass::SecondaryActionPressed);
		EnhancedInputComponent->BindAction(InputData->SecondaryAction, ETriggerEvent::Completed, this, &ThisClass::SecondaryActionReleased);
		EnhancedInputComponent->BindAction(InputData->SprintAction, ETriggerEvent::Started, this, &ThisClass::SprintActionPressed);
		EnhancedInputComponent->BindAction(InputData->SprintAction, ETriggerEvent::Completed, this, &ThisClass::SprintActionReleased);
		EnhancedInputComponent->BindAction(InputData->InteractAction, ETriggerEvent::Started, this, &ThisClass::InteractActionPressed);
		EnhancedInputComponent->BindAction(InputData->InteractAction, ETriggerEvent::Completed, this, &ThisClass::InteractActionReleased);
		EnhancedInputComponent->BindAction(InputData->ReloadAction, ETriggerEvent::Started, this, &ThisClass::ReloadActionPressed);
		EnhancedInputComponent->BindAction(InputData->CancelPreviewAction, ETriggerEvent::Started, this, &ThisClass::CancelPreviewActionPressed);
		EnhancedInputComponent->BindAction(InputData->EquipPrimaryAction, ETriggerEvent::Started, this, &ThisClass::EquipPrimaryActionPressed);
		EnhancedInputComponent->BindAction(InputData->EquipUtilityAction, ETriggerEvent::Started, this, &ThisClass::EquipUtilityActionPressed);
		EnhancedInputComponent->BindAction(InputData->EquipThrowableAction, ETriggerEvent::Started, this, &ThisClass::EquipThrowableActionPressed);
		EnhancedInputComponent->BindAction(InputData->OpenPauseMenuAction, ETriggerEvent::Started, this, &ThisClass::OpenPauseMenuActionPressed);
	}
}

// For the server, ASC ability actor info is initialized here
// At this point, we know the character has been possessed by a controller (not too early to initialize)
void AComplyPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	checkf(PlayerData, TEXT("PlayerData not set on %s"), *GetName());
	checkf(InputData, TEXT("InputData not set on %s"), *GetName());
	
	if (!GetAbilitySystemComponent() || !HasAuthority()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	WeaponMesh->SetStaticMesh(PlayerData->PrimaryMesh);
	
	InitializeAttributes();
	
	// Only give startup abilities if not in the lobby map
	const FString MapName = GetWorld()->GetMapName();
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
	
	checkf(PlayerData, TEXT("PlayerData not set on %s"), *GetName());
	checkf(InputData, TEXT("InputData not set on %s"), *GetName());
	
	if (!GetAbilitySystemComponent()) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
	
	WeaponMesh->SetStaticMesh(PlayerData->PrimaryMesh);
	
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
	
	if (IsLocallyControlled()) { TraceForInteractable(); }
	
	RotatePlayerToLook(DeltaTime);
	
	if (PlayerData)
	{
		CameraBoom->SocketOffset.X = FMath::FInterpTo(
			CameraBoom->SocketOffset.X, BaseSocketOffsetX, DeltaTime, PlayerData->RecoilReturnSpeed
		);
	}
}

UAbilitySystemComponent* AComplyPlayerCharacter::GetAbilitySystemComponent() const
{
	const AComplyPlayerState* ComplyPlayerState = Cast<AComplyPlayerState>(GetPlayerState());
	if (!IsValid(ComplyPlayerState)) return nullptr;
	
	return ComplyPlayerState->GetAbilitySystemComponent();
}

void AComplyPlayerCharacter::InitializeAttributes() const
{
    if (!PlayerData) return;

    checkf(IsValid(PlayerData->InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set"));

    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();

    // Base attributes shared across all players
    const FGameplayEffectSpecHandle BaseSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(PlayerData->InitializeAttributesEffect, 1.f, ContextHandle);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(BaseSpecHandle, ComplyTags::SetByCaller::Stats::SBC_MaxHealth, PlayerData->MaxHealth);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(BaseSpecHandle, ComplyTags::SetByCaller::Stats::SBC_MaxArmor, PlayerData->MaxArmor);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(BaseSpecHandle, ComplyTags::SetByCaller::Stats::SBC_MovementSpeed, PlayerData->MovementSpeed);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*BaseSpecHandle.Data.Get());

    // Ranger
    if (const URangerData* RangerData = Cast<URangerData>(PlayerData))
    {
        const FGameplayEffectSpecHandle WeaponSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(RangerData->InitializeWeaponAttributesEffect, 1.f, ContextHandle);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_RifleMaxAmmo, RangerData->RifleMaxAmmo);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_RifleMaxReserveAmmo, RangerData->RifleMaxReserveAmmo);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_PlasmaGrenadeMaxCharges, RangerData->PlasmaGrenadeMaxCharges);
        GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*WeaponSpecHandle.Data.Get());
    }
    // Enforcer
    else if (const UEnforcerData* EnforcerData = Cast<UEnforcerData>(PlayerData))
    {
        const FGameplayEffectSpecHandle WeaponSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EnforcerData->InitializeWeaponAttributesEffect, 1.f, ContextHandle);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_MagnumMaxAmmo, EnforcerData->MagnumMaxAmmo);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_MagnumMaxReserveAmmo, EnforcerData->MagnumMaxReserveAmmo);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_TurretMaxCharges, EnforcerData->TurretMaxCharges);
        GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*WeaponSpecHandle.Data.Get());
    }
    // Disruptor
    else if (const UDisruptorData* DisruptorData = Cast<UDisruptorData>(PlayerData))
    {
        const FGameplayEffectSpecHandle WeaponSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DisruptorData->InitializeWeaponAttributesEffect, 1.f, ContextHandle);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_ShotgunMaxAmmo, DisruptorData->ShotgunMaxAmmo);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_ShotgunMaxReserveAmmo, DisruptorData->ShotgunMaxReserveAmmo);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(WeaponSpecHandle, ComplyTags::SetByCaller::Stats::SBC_DecoyGrenadeMaxCharges, DisruptorData->DecoyGrenadeMaxCharges);
        GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*WeaponSpecHandle.Data.Get());
    }
}

URangedWeaponAbilityBase* AComplyPlayerCharacter::GetEquippedPrimaryWeapon() const
{
	if (!GetAbilitySystemComponent()) return nullptr;
	
	for (const FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass() == EquippedPrimaryWeaponClass)
		{
			return Cast<URangedWeaponAbilityBase>(Spec.GetPrimaryInstance() ? Spec.GetPrimaryInstance() : Spec.Ability.Get());
		}
	}
	return nullptr;
}

UThrowableAbilityBase* AComplyPlayerCharacter::GetEquippedThrowable() const
{
	if (!GetAbilitySystemComponent()) return nullptr;

	for (const FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass()->IsChildOf(EquippedThrowableClass))
		{
			return Cast<UThrowableAbilityBase>(Spec.GetPrimaryInstance() ? Spec.GetPrimaryInstance() : Spec.Ability.Get());
		}
	}
	
	return nullptr;
}

UUtilityAbilityBase* AComplyPlayerCharacter::GetEquippedUtility() const
{
	if (!GetAbilitySystemComponent()) return nullptr;

	for (const FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (Spec.Ability->GetClass()->IsChildOf(EquippedUtilityClass))
		{
			return Cast<UUtilityAbilityBase>(Spec.GetPrimaryInstance() ? Spec.GetPrimaryInstance() : Spec.Ability.Get());
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
	const FVector Start = GetCapsuleComponent()->GetComponentLocation();
	const FVector End = Start - FVector(0.f, 0.f, 500.f);
	GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility);
    
	if (GroundHit.bBlockingHit)
	{
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		SetActorLocation(GroundHit.ImpactPoint + FVector(0.f, 0.f, CapsuleHalfHeight));
	}
	
	// Restore relevant attributes to defaults before death
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ReviveEffectClass, 1.f, ContextHandle);
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

void AComplyPlayerCharacter::SpawnImpactEffectsLocal(const FVector& ImpactPoint, const FVector& ImpactNormal, const FVector& MuzzleLocation, UComplyWeaponData* WeaponData)
{
	if (!WeaponData) return;

	// Spawns a random decal from an array
	const TArray<TObjectPtr<UMaterialInstance>>& Decals = WeaponData->BulletImpactDecals;
	if (Decals.IsEmpty()) return;

	const int32 DecalIndex = FMath::RandRange(0, Decals.Num() - 1);
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(), Decals[DecalIndex], FVector(5.f, 10.f, 10.f), ImpactPoint,
		UKismetMathLibrary::MakeRotFromX(ImpactNormal), 5.f
	);

	if (Decal)
	{
		Decal->SetFadeScreenSize(0.f);
		Decal->SetFadeOut(5.f, 1.f, true);
	}

	// Spawns a tracer niagara effect and sets its beam end to the impact point
	UNiagaraComponent* Tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), WeaponData->BulletTracerEffect, MuzzleLocation, FRotator::ZeroRotator, FVector(1.f)
	);
			
	if (Tracer)
	{
		// Local offset from the muzzle, Niagara was interpreting world space vector as local relative to the spawn location
		const FVector BeamEndLocal = ImpactPoint - MuzzleLocation;
		Tracer->SetVariableVec3(FName("BeamEnd"), BeamEndLocal);
	}
}

void AComplyPlayerCharacter::Client_ShowDamageNumber_Implementation(float DamageAmount, FVector WorldPos)
{
	if (const AComplyPlayerController* PC = Cast<AComplyPlayerController>(GetController()))
	{
		PC->DamageNumbersWidget->ShowDamageNumber(DamageAmount, WorldPos);
	}
}

void AComplyPlayerCharacter::ApplyFiringFeedback(const UComplyWeaponData* WeaponData)
{
	if (!WeaponData) return;
    
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraShake(WeaponData->FiringCameraShake);
		}
	}
    
	CameraBoom->SocketOffset.X -= WeaponData->RecoilKickDistance;
}

void AComplyPlayerCharacter::Server_SetIsFiring_Implementation(bool bFiring, float HeldDuration)
{
	bIsFiring = bFiring;
    
	if (bFiring)
	{
		// Large number so the server doesn't stop early before the release RPC arrives
		FireInputHeldDuration = TNumericLimits<float>::Max();
	}
	else
	{
		FireInputHeldDuration = HeldDuration;
	}
}

// Multicast that handles broadcasting impact decals and bullet tracers from the muzzle to the impact point
void AComplyPlayerCharacter::Multicast_SpawnImpactEffects_Implementation(FVector ImpactPoint, FVector ImpactNormal,
	FVector MuzzleLocation, UComplyWeaponData* WeaponData)
{
	// Decal and effects are spawned locally for clients, don't multicast to them again
	if (IsLocallyControlled()) return;
	
	SpawnImpactEffectsLocal(ImpactPoint, ImpactNormal, MuzzleLocation, WeaponData);
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
	const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
		GetActorLocation(), Target->GetActorLocation());
	SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
}

void AComplyPlayerCharacter::SetEquippedPrimaryWeapon(TSubclassOf<URangedWeaponAbilityBase> NewWeaponClass)
{
	EquippedPrimaryWeaponClass = NewWeaponClass;
}

void AComplyPlayerCharacter::SetEquippedThrowable(TSubclassOf<UThrowableAbilityBase> NewWeaponClass)
{
	EquippedThrowableClass = NewWeaponClass;
}

void AComplyPlayerCharacter::SetEquippedUtility(TSubclassOf<UThrowableAbilityBase> NewWeaponClass)
{
	EquippedUtilityClass = NewWeaponClass;
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
			
			if (!GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_FiringBlocked))
			{
				bFiredThisFrame = true;
			}
		}
	}
	
	// if (ApplyFireEffectAbilityClass)
	// {
	// 	if (!GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_FiringBlocked))
	// 	{
	// 		GetAbilitySystemComponent()->TryActivateAbilityByClass(ApplyFireEffectAbilityClass);
	// 	}
	// }
	
	FireInputStartTime = GetWorld()->GetTimeSeconds();
	
	bIsFiring = true;
	Server_SetIsFiring(true, 0.f);

	bFireInputHeld = true;
}

void AComplyPlayerCharacter::PrimaryActionReleased()
{
	if (!GetAbilitySystemComponent()) return;
    
	for (FGameplayAbilitySpec& Spec : GetAbilitySystemComponent()->GetActivatableAbilities())
	{
		// Confirm the throw on release only for throwables that don't manage their own placement flow
		if (Spec.Ability->GetAssetTags().HasTagExact(ComplyTags::ComplyAbilities::Throwable)
			&& Spec.IsActive()
			&& !Cast<UThrowableAbilityBase>(Spec.GetPrimaryInstance())->bUsesCustomPreviewLogic)
		{
			GetAbilitySystemComponent()->LocalInputConfirm();
			break;
		}
	}
    
	bIsFiring = false;
	const float HeldDuration = GetWorld()->GetTimeSeconds() - FireInputStartTime;
	Server_SetIsFiring(false, HeldDuration);

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
			if (Spec.IsActive())
			{
				GetAbilitySystemComponent()->CancelAbilityHandle(Spec.Handle);
			}
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
		const int32 TotemStacks = GetAbilitySystemComponent()->GetTagCount(ComplyTags::States::State_TotemBuffed);
		float CorrectSpeed = 500.f + (TotemStacks * PlayerData->TotemSpeedBonusPerStack);
		
		if (GetAbilitySystemComponent()->HasMatchingGameplayTag(ComplyTags::States::State_Slowed))
		{
			CorrectSpeed -= NextSlowMagnitude;
		}
        
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
						PlayAnimMontage(PlayerData->PrepareReloadMontage);
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

void AComplyPlayerCharacter::OpenPauseMenuActionPressed()
{
	if (AComplyPlayerController* PC = Cast<AComplyPlayerController>(GetController()))
	{
		PC->OpenMenuWidget(PauseMenuWidgetClass);
	}
}

void AComplyPlayerCharacter::RotatePlayerToLook(float DeltaTime)
{
	
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		const FRotator ControlRot = PC->GetControlRotation();
		const FRotator CurrentRot = GetActorRotation();
	
		const float YawDiff = FMath::UnwindDegrees(ControlRot.Yaw - CurrentRot.Yaw);
		const float NewYaw = CurrentRot.Yaw + FMath::Clamp(YawDiff, -20.f * DeltaTime * 360.f, 20.f * DeltaTime * 360.f);

		SetActorRotation(FRotator(0.f, NewYaw, 0.f));
	}
}

void AComplyPlayerCharacter::ZoomIn(float DeltaTime) const
{
	if (!IsLocallyControlled()) return;
	UCameraComponent* CameraComp = FindComponentByClass<UCameraComponent>();
	CameraComp->FieldOfView = FMath::FInterpTo(
		CameraComp->FieldOfView, PlayerData->AimFOV, DeltaTime, PlayerData->ZoomSpeed);
}

void AComplyPlayerCharacter::ZoomOut(float DeltaTime) const
{
	if (!IsLocallyControlled()) return;
	UCameraComponent* CameraComp = FindComponentByClass<UCameraComponent>();
	CameraComp->FieldOfView = FMath::FInterpTo(
		CameraComp->FieldOfView, PlayerData->DefaultFOV, DeltaTime, PlayerData->ZoomSpeed);
}

// Traces to the camera each tick, checking for interactable actors
void AComplyPlayerCharacter::TraceForInteractable()
{
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, this, 200.f, TraceStart, TraceEnd, TraceDirection)) return;
	
	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility);

	AActor* HitActor = bHit ? Hit.GetActor() : nullptr;
	IInteractableInterface* Interactable = HitActor ? Cast<IInteractableInterface>(HitActor) : nullptr;

	if (Interactable != CurrentFocusedInteractable)
	{
		if (CurrentFocusedInteractable) CurrentFocusedInteractable->HideInteractionPrompt();

		CurrentFocusedInteractable = Interactable;

		if (CurrentFocusedInteractable) CurrentFocusedInteractable->ShowInteractionPrompt();
	}
}

void AComplyPlayerCharacter::OnAimingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount > 0;
}

// If friendly fire is on and the distracted tag was applied by the decoy grenade, apply flashbang effect to affected players
void AComplyPlayerCharacter::OnDistractedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount <= 0) return;
	if (!IsLocallyControlled()) return;
	
	UComplyGameInstance* GI = GetWorld()->GetGameState<UComplyGameInstance>();
	if (GI && GI->bFriendlyFire)
	{
		if (AComplyPlayerController* PC = Cast<AComplyPlayerController>(GetController())) PC->ShowFlashbangEffect();
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
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(TotemSpeedBuffEffectClass, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, 
		ComplyTags::SetByCaller::SBC_TotemSpeedBuff, PlayerData->TotemSpeedBonusPerStack * NewCount
	);
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
		case EWeaponSlot::Primary:   return PlayerData->PrimaryMeshScale;
		case EWeaponSlot::Utility:   return PlayerData->UtilityMeshScale;
		case EWeaponSlot::Throwable: return PlayerData->ThrowableMeshScale;
	}
	return FVector::ZeroVector;
}

UStaticMesh* AComplyPlayerCharacter::GetMeshForSlot(EWeaponSlot Slot)
{
	switch (Slot)
	{
		case EWeaponSlot::None:		 return nullptr;
		case EWeaponSlot::Primary:   return PlayerData->PrimaryMesh;
		case EWeaponSlot::Utility:   return PlayerData->UtilityMesh;
		case EWeaponSlot::Throwable: return PlayerData->ThrowableMesh;
	}
	return nullptr;
}

void AComplyPlayerCharacter::OnWeaponEquipped(EWeaponSlot Slot)
{
	if (Slot == CurrentEquippedSlot) return;

	CurrentEquippedSlot = Slot;
    
	GetAbilitySystemComponent()->SetLooseGameplayTagCount(ComplyTags::States::State_Equipping, 1);

	UAnimMontage* Montage = nullptr;
	UTexture2D* CrosshairTexture = nullptr;
	switch (Slot)
	{
		case EWeaponSlot::Primary:
			Montage = PlayerData->PrimaryEquipMontage;
			CrosshairTexture = GetEquippedPrimaryWeapon()->WeaponData->CrosshairTexture;
			break;
		case EWeaponSlot::Utility:
			Montage = PlayerData->UtilityEquipMontage;
			CrosshairTexture = GetEquippedUtility()->UtilityData->CrosshairTexture;
			break;
		case EWeaponSlot::Throwable:
			Montage = PlayerData->ThrowableEquipMontage;
			CrosshairTexture = GetEquippedThrowable()->GrenadeData->CrosshairTexture;
			break;
		default: break;
	}

	if (Montage) PlayAnimMontage(Montage);

	if (!ensureMsgf(CrosshairTexture, TEXT("CrosshairTexture is null for slot %d. Check data asset."), static_cast<int32>(Slot))) return;

	if (const AComplyPlayerController* PC = GetController<AComplyPlayerController>())
	{
		if (!PC->HUDWidget) return;
		if (UComplyCrosshairWidget* Crosshair = PC->HUDWidget->GetCrosshairWidget())
		{
			Crosshair->SetCrosshairTexture(CrosshairTexture);
		}
	}
}

// OnRep handles playing the equip animation for clients
void AComplyPlayerCharacter::OnRep_CurrentEquippedSlot()
{
	UAnimMontage* Montage = nullptr;
	switch (CurrentEquippedSlot)
	{
		case EWeaponSlot::None:		 Montage = nullptr;
		case EWeaponSlot::Primary:   Montage = PlayerData->PrimaryEquipMontage;   break;
		case EWeaponSlot::Utility:   Montage = PlayerData->UtilityEquipMontage;   break;
		case EWeaponSlot::Throwable: Montage = PlayerData->ThrowableEquipMontage; break;
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
