// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/ReloadAbilities/ReloadAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


void UReloadAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	HandleReload();
}

void UReloadAbility::HandleReload()
{
	// Find the active ranged weapon to get its montage, ammo reload effect and to activate it later
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		ActiveWeapon = Character->GetEquippedPrimaryWeapon();
		Character->bIsReloading = true;
	}
	
	const UWeaponAttributeSet* WeaponAS = GetAbilitySystemComponentFromActorInfo()->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	
	float CurrentAmmoInMag = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentAmmoAttribute(), bFound);
	float MaxAmmoInMag = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetMaxAmmoAttribute(), bFound);
	float ReserveAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentReserveAmmoAttribute(), bFound);
	
	// Can't reload if there's no more reserve ammo
	if (WeaponAS && ReserveAmmo <= 0.f)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
		CueParams.Instigator = GetAvatarActorFromActorInfo();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::WeaponDryFire, CueParams);
		
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	// Can't reload if the mag is full
	if (WeaponAS && FMath::IsNearlyEqual(CurrentAmmoInMag, MaxAmmoInMag))
	{
	  	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	if (PlayActivationMontageTask)
	{
		PlayActivationMontageTask->EndTask();
		PlayActivationMontageTask = nullptr;
	}
	
	if (ReloadMontageTask)
	{
		ReloadMontageTask->EndTask();
	}
	
	// If the weapon is not active, don't try to call functions on it externally
	if (!ActiveWeapon)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
	CueParams.Instigator = GetAvatarActorFromActorInfo();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::WeaponReload, CueParams);
	
	// Play reload animation
	ReloadMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ActiveWeapon->ReloadMontage, 1.f, NAME_None, true);
		
	ReloadMontageTask->OnCompleted.AddDynamic(this, &UReloadAbility::OnReloadMontageCompleted);
	ReloadMontageTask->OnInterrupted.AddDynamic(this, &UReloadAbility::OnReloadMontageCompleted);
	ReloadMontageTask->OnCancelled.AddDynamic(this, &UReloadAbility::OnReloadMontageCompleted);
	ReloadMontageTask->OnBlendOut.AddDynamic(this, &UReloadAbility::OnReloadMontageCompleted);
		
	ReloadMontageTask->ReadyForActivation();
		
	// Effect that applies the reloading tag
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReloadStateEffectClass, 1.f, ContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UReloadAbility::OnReloadMontageCompleted()
{
	if (ActiveWeapon)
	{
		float AmmoSpent = 0.f;
		bool bFound = false;
		const float CurrentAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentAmmoAttribute(), bFound);
		const float MaxAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetMaxAmmoAttribute(), bFound);
		AmmoSpent = MaxAmmo - CurrentAmmo;

		FGameplayEffectContextHandle ReserveAmmoContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FGameplayEffectSpecHandle ReserveAmmoSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ActiveWeapon->ReduceReserveAmmoEffectClass, 1.f, ReserveAmmoContextHandle);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(ReserveAmmoSpecHandle, ActiveWeapon->GetReduceReserveAmmoTag(), -AmmoSpent);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ReserveAmmoSpecHandle.Data.Get());

		FGameplayEffectContextHandle CurrentAmmoContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FGameplayEffectSpecHandle CurrentAmmoSpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ActiveWeapon->ReloadEffectClass, 1.f, CurrentAmmoContextHandle);
		GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*CurrentAmmoSpecHandle.Data);
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
	CueParams.Instigator = GetAvatarActorFromActorInfo();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::WeaponReloadFinished, CueParams);

	// Remove reloading tag
	FGameplayTagContainer Tags;
	Tags.AddTag(ComplyTags::States::State_Reloading);
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);

	// Resume firing if still holding, otherwise end the ability to allow future inputs (as it's instanced per actor)
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);
	if (Tags.HasTagExact(ComplyTags::States::State_Firing) && ActiveWeapon)
	{
		if (ActiveWeapon)
		{
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilityByClass(ActiveWeapon->GetClass());
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UReloadAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		Character->bIsReloading = false;
	}
	
	// Ensure reloading tag gets removed
	FGameplayTagContainer Tags;
	Tags.AddTag(ComplyTags::States::State_Reloading);
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);
	
	GetAbilitySystemComponentFromActorInfo()->SetLooseGameplayTagCount(ComplyTags::States::State_FiringBlocked, 0);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
