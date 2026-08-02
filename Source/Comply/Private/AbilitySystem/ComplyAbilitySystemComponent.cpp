// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "Actors/AbilityActors/PlasmaGrenade/PlasmaGrenade.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Utility_Disruptor.h"
#include "AbilitySystem/Abilities/Player/Enforcer/Throwable_Enforcer.h"
#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "AbilitySystem/Data/Player/Abilities/Turret/DeployableTurretAbilityData.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/BuffTotemUtilityData.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/ComplyUtilityData.h"
#include "AbilitySystem/Data/Player/Grenades/ComplyGrenadeData.h"
#include "AbilitySystem/Data/Player/Grenades/DecoyGrenadeData.h"
#include "AbilitySystem/Data/Player/Grenades/PlasmaGrenadeData.h"
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
	
	checkf(Ability->GrenadeData, TEXT("GrenadeData not set on %s"), *GetName());
	
	bool bFound = false;
	const float GrenadeCurrentCharges = GetGameplayAttributeValue(
		UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute(), bFound
	);
    
	// Don't spawn a grenade if there are no charges
	if (GrenadeCurrentCharges <= 0.f) return;

	// Cost is applied in the RPC, so the next RPC will see the updated data
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Ability->GrenadeData->CostEffectClass, 1.f, MakeEffectContext());
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
		Ability->GrenadeData->GrenadeActorClass,
		SpawnTransform, GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Grenade)
	{
		Grenade->GrenadeData = Cast<UPlasmaGrenadeData>(Ability->GrenadeData);
		Grenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		const FVector SafeLaunchVelocity = InLaunchVelocity.GetClampedToMaxSize(Ability->GrenadeData->ThrowSpeed);
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
	
	checkf(Ability->GrenadeData, TEXT("GrenadeData not set on %s"), *GetName());
	
	bool bFound = false;
	const float GrenadeCurrentCharges = GetGameplayAttributeValue(
		UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute(), bFound
	);
    
	// Don't spawn a grenade if there are no charges
	if (GrenadeCurrentCharges <= 0.f) return;

	// Cost is applied in the RPC, so the next RPC will see the updated data
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(Ability->GrenadeData->CostEffectClass, 1.f, MakeEffectContext());
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
		Ability->GrenadeData->GrenadeActorClass,
		SpawnTransform, GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (DecoyGrenade)
	{
		DecoyGrenade->GrenadeData = Cast<UDecoyGrenadeData>(Ability->GrenadeData);
		DecoyGrenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		const FVector SafeLaunchVelocity = InLaunchVelocity.GetClampedToMaxSize(Ability->GrenadeData->ThrowSpeed);
		DecoyGrenade->LaunchVelocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(DecoyGrenade, SpawnTransform);
	}
}

void UComplyAbilitySystemComponent::Server_PlaceTurret_Implementation(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation)
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	UThrowable_Enforcer* Ability = Cast<UThrowable_Enforcer>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	checkf(Ability->TurretData, TEXT("TurretData not set on %s"), *GetName());
	
	// Commit cost and cooldown server-side before doing anything else
	// It must be committed here because the whole call stack leading up to placing the turret runs only locally
	if (!Ability->CommitAbility(AbilityHandle, Ability->GetCurrentActorInfo(), Ability->GetCurrentActivationInfo()))
	{
		return;
	}
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());

	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

	ADeployableTurret* Turret = GetWorld()->SpawnActorDeferred<ADeployableTurret>(
		Ability->TurretData->TurretActorClass,
		SpawnTransform, GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Turret)
	{
		Turret->TurretData = Ability->TurretData;
		Turret->SetLifeSpan(Ability->TurretData->TurretLifetime);
		Turret->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActor());

		UGameplayStatics::FinishSpawningActor(Turret, SpawnTransform);
	}
}

void UComplyAbilitySystemComponent::Server_PlaceBuffTotem_Implementation(FGameplayAbilitySpecHandle AbilityHandle,
                                                                      FVector SpawnLocation, float BuffTotemLifetime)
{
	const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityHandle);
	UUtilityAbilityBase* Ability = Cast<UUtilityAbilityBase>(Spec->GetPrimaryInstance());
	
	if (!Spec || !Ability) return;
	
	checkf(Ability->UtilityData, TEXT("UtilityData not set on %s"), *GetName());
	
	// Commit cost and cooldown server-side before doing anything else
	// It must be committed here because the whole call stack leading up to placing the turret runs only locally
	if (!Ability->CommitAbility(AbilityHandle, Ability->GetCurrentActorInfo(), Ability->GetCurrentActivationInfo()))
	{
		return;
	}
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActor());
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(InstigatorPawn);

	ABuffTotem* BuffTotem = GetWorld()->SpawnActorDeferred<ABuffTotem>(
		Ability->UtilityData->UtilityActorClass,
		FTransform(FRotator::ZeroRotator, SpawnLocation),
		GetOwnerActor(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (BuffTotem)
	{
		BuffTotem->BuffTotemData = Cast<UBuffTotemUtilityData>(Ability->UtilityData);
		BuffTotem->SetLifeSpan(Ability->UtilityData->UtilityLifetime);

		UGameplayStatics::FinishSpawningActor(BuffTotem, FTransform(FRotator::ZeroRotator, SpawnLocation));
	}
}
