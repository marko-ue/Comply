// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "Actors/AbilityActors/PlasmaGrenade/PlasmaGrenade.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Utility_Disruptor.h"
#include "AbilitySystem/Abilities/Player/Enforcer/Throwable_Enforcer.h"
#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/AbilityActors/BuffTotem/BuffTotem.h"
#include "Actors/AbilityActors/DecoyGrenade/DecoyGrenade.h"
#include "Actors/AbilityActors/DeployableTurret/DeployableTurret.h"
#include "Interface/Player/WeaponInterface.h"
#include "Kismet/GameplayStatics.h"


UComplyAbilitySystemComponent::UComplyAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UComplyAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UComplyAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UComplyAbilitySystemComponent::Server_ThrowPlasmaGrenade_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation, FVector InLaunchVelocity)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	UThrowable_Ranger* Ability = Cast<UThrowable_Ranger>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	bool bFound = false;
	float GrenadeCurrentCharges = GetGameplayAttributeValue(
		UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute(), bFound);
    
	// Don't spawn a grenade if there are no charges
	if (GrenadeCurrentCharges <= 0.f) return;

	// Cost is applied in the RPC, so the next RPC will see the updated data
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Ability->CostEffectClass, 1.f, MakeEffectContext());
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	AActor* Avatar = GetAvatarActor();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	Ability->EquipWeaponBasedOnCharges(WeaponOwner, this);
	
	// Makes EndAbility get called on clients 
	// Needed because the path that ends the ability never runs on the server for remote clients (OnThrowMontageCompleted)
	CancelAbilitySpec(*Spec, nullptr);
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(
		Ability->ThrowableActorClass,
		SpawnTransform, GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Grenade)
	{
		Grenade->ExplosionRadius = Ability->ExplosionRadius;
		Grenade->MaxDamage = Ability->Damage.GetValueAtLevel(Ability->GetAbilityLevel());
		Grenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		Grenade->DamageEffectClass = Ability->DamageEffectClass;
		Grenade->DamageTypeTag = Ability->DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = InLaunchVelocity.GetClampedToMaxSize(Ability->ThrowSpeed);
		Grenade->LaunchVelocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(Grenade, SpawnTransform);
	}
}

void UComplyAbilitySystemComponent::Server_ThrowDecoyGrenade_Implementation(FGameplayAbilitySpecHandle AbilityHandle,
	FVector SpawnLocation, FRotator SpawnRotation, FVector InLaunchVelocity)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	UThrowable_Disruptor* Ability = Cast<UThrowable_Disruptor>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	bool bFound = false;
	float GrenadeCurrentCharges = GetGameplayAttributeValue(
		UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute(), bFound);
    
	// Don't spawn a grenade if there are no charges
	if (GrenadeCurrentCharges <= 0.f) return;

	// Cost is applied in the RPC, so the next RPC will see the updated data
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Ability->CostEffectClass, 1.f, MakeEffectContext());
	ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	AActor* Avatar = GetAvatarActor();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	Ability->EquipWeaponBasedOnCharges(WeaponOwner, this);
	
	// Makes EndAbility get called on clients 
	// Needed because the path that ends the ability never runs on the server for remote clients (OnThrowMontageCompleted)
	CancelAbilitySpec(*Spec, nullptr);
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	
	ADecoyGrenade* DecoyGrenade = GetWorld()->SpawnActorDeferred<ADecoyGrenade>(
		Ability->ThrowableActorClass,
		SpawnTransform, GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (DecoyGrenade)
	{
		DecoyGrenade->PullRadius = Ability->PullRadius;
		DecoyGrenade->DecoyGrenadeLifetime = Ability->DecoyGrenadeLifetime;
		DecoyGrenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		DecoyGrenade->DamageEffectClass = Ability->DamageEffectClass;
		DecoyGrenade->DamageTypeTag = Ability->DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = InLaunchVelocity.GetClampedToMaxSize(Ability->ThrowSpeed);
		DecoyGrenade->LaunchVelocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(DecoyGrenade, SpawnTransform);
	}
}

void UComplyAbilitySystemComponent::Server_PlaceTurret_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation)
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	UThrowable_Enforcer* Ability = Cast<UThrowable_Enforcer>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	// Commit cost and cooldown server-side before doing anything else
	// It must be committed here because the whole call stack leading up to placing the turret runs only locally
	if (!Ability->CommitAbility(AbilityHandle, Ability->GetCurrentActorInfo(), Ability->GetCurrentActivationInfo()))
	{
		return;
	}
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ADeployableTurret* Turret = GetWorld()->SpawnActorDeferred<ADeployableTurret>(
		Ability->TurretActorClass,
		SpawnTransform, GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Turret)
	{
		Turret->Damage = Ability->Damage.GetValueAtLevel(Ability->GetAbilityLevel());
		Turret->SetLifeSpan(Ability->TurretLifetime);
		Turret->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		Turret->DamageEffectClass = Ability->DamageEffectClass;
		Turret->DamageTypeTag = Ability->DamageType;

		UGameplayStatics::FinishSpawningActor(Turret, SpawnTransform);
	}
}

void UComplyAbilitySystemComponent::Server_PlaceBuffTotem_Implementation(FGameplayAbilitySpecHandle AbilityHandle,
                                                                      FVector SpawnLocation, float BuffTotemLifetime)
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	UUtility_Disruptor* Ability = Cast<UUtility_Disruptor>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	// Commit cost and cooldown server-side before doing anything else
	// It must be committed here because the whole call stack leading up to placing the turret runs only locally
	if (!Ability->CommitAbility(AbilityHandle, Ability->GetCurrentActorInfo(), Ability->GetCurrentActivationInfo()))
	{
		return;
	}
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(InstigatorPawn);

	ABuffTotem* BuffTotem = GetWorld()->SpawnActor<ABuffTotem>(
		Ability->BuffTotemActorClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	if (BuffTotem)
	{
		BuffTotem->SetLifeSpan(BuffTotemLifetime);
	}
}
