// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "GameplayEffect.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "Net/UnrealNetwork.h"


AComplyCharacterBase::AComplyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Prevents the character movement component from applying impulses to physics objects and characters
	GetCharacterMovement()->bEnablePhysicsInteraction = false;
	GetCharacterMovement()->bPushForceScaledToMass    = false;
	GetCharacterMovement()->PushForceFactor           = 0.f;
	GetCharacterMovement()->InitialPushForceFactor    = 0.f;
	GetCharacterMovement()->RepulsionForce            = 0.f;
}

UAbilitySystemComponent* AComplyCharacterBase::GetAbilitySystemComponent() const
{
	return nullptr;
}

void AComplyCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bIsDead);
}

void AComplyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AComplyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AComplyCharacterBase::InitializeAttributes() const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set"));
	checkf(IsValid(InitializeWeaponAttributesEffect), TEXT("InitializeWeaponAttributesEffect not set"));
	
	const FGameplayEffectContextHandle AttributesContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle AttributesSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, AttributesContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*AttributesSpecHandle.Data.Get());
	
	const FGameplayEffectContextHandle WeaponAttributesContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle WeaponAttributesSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeWeaponAttributesEffect, 1.f, WeaponAttributesContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*WeaponAttributesSpecHandle.Data.Get());
}

void AComplyCharacterBase::GiveStartupAbilities()
{
	if (!GetAbilitySystemComponent()) return;
	
	for (const FAbilitySet& Set: StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(Set.AbilityClass);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Set.InputTag);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Set.AbilityTypeTag);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
		
		// Store equipped weapon classes
		if (Set.AbilityClass->IsChildOf(URangedWeaponAbilityBase::StaticClass()))
		{
			if (AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(this))
			{
				PlayerCharacter->EquippedPrimaryWeaponClass = Set.AbilityClass;
			}
		}
		if (Set.AbilityClass->IsChildOf(UThrowableAbilityBase::StaticClass()))
		{
			if (AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(this))
			{
				PlayerCharacter->EquippedThrowableClass = Set.AbilityClass;
			}
		}
		if (Set.AbilityClass->IsChildOf(UUtilityAbilityBase::StaticClass()))
        {
        	if (AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(this))
        	{
        		PlayerCharacter->EquippedUtilityClass = Set.AbilityClass;
        	}
        }
	}
}

void AComplyCharacterBase::ClearStartupAbilities()
{
	if (!GetAbilitySystemComponent()) return;

	for (const FAbilitySet& Set : StartupAbilities)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = GetAbilitySystemComponent()->FindAbilitySpecFromClass(Set.AbilityClass))
		{
			GetAbilitySystemComponent()->ClearAbility(AbilitySpec->Handle);
		}
	}
}

void AComplyCharacterBase::HandleHit(const AActor* HitActor)
{
	if (HitActor && HitActor->Implements<UPlayerInterface>())
	{
		if (GetAbilitySystemComponent())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
			GetAbilitySystemComponent()->ExecuteGameplayCue(ComplyTags::GameplayCues::PlayerHit, CueParams);
		}
	}
	
	Multicast_PlayHitReact();
}

void AComplyCharacterBase::Multicast_PlayHitReact_Implementation()
{
	PlayAnimMontage(HitReactMontage);
}

void AComplyCharacterBase::Die(AActor* DeadActor)
{
	if (DeadActor && DeadActor->Implements<UPlayerInterface>())
	{
		if (GetAbilitySystemComponent())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
			GetAbilitySystemComponent()->ExecuteGameplayCue(ComplyTags::GameplayCues::PlayerDeath, CueParams);
		}
		
		if (AComplyPlayerCharacter* DeadPlayer = Cast<AComplyPlayerCharacter>(DeadActor))
		{
			DeadPlayer->DownPlayer();
		}
	}
	else if (DeadActor && DeadActor->Implements<UEnemyInterface>())
	{
		// Prevents multiple cues and function calls if multiple sources kill the enemy in the same frame (like shotgun pellets)
		if (bIsDead) return;
		bIsDead = true; // Replicated variable triggers OnRep
		
		FGameplayCueParameters CueParams;
		CueParams.Location = DeadActor->GetActorLocation();
		CueParams.Instigator = DeadActor;
		GetAbilitySystemComponent()->ExecuteGameplayCue(ComplyTags::GameplayCues::EnemyDeath, CueParams);
		
		OnRep_IsDead();
	}
}

void AComplyCharacterBase::OnRep_IsDead()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
    
	if (AController* C = GetController())
	{
		C->UnPossess();
	}

	SetLifeSpan(3.f);
}

void AComplyCharacterBase::ActivateInitialAbility() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC) return;

	// Reset all block counts first
	FGameplayTagContainer AllWeaponTags;
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Primary);
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Utility);
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Throwable);
	ASC->UnBlockAbilitiesWithTags(AllWeaponTags);

	// Cancel everything, then block all except this ability
	ASC->CancelAbilities(&AllWeaponTags, nullptr);

	// Unblock the relevant ability related to this equip ability
	FGameplayTagContainer TagsToBlock = AllWeaponTags;
	TagsToBlock.RemoveTag(ComplyTags::ComplyAbilities::Primary);
	ASC->BlockAbilitiesWithTags(TagsToBlock);
}

void AComplyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
