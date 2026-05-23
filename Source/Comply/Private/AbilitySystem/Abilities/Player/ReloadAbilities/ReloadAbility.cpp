// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/ReloadAbilities/ReloadAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Character/ComplyPlayerCharacter.h"

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
	const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	ActiveWeapon = Character->GetEquippedPrimaryWeapon();
	
	const UWeaponAttributeSet* WeaponAS = GetAbilitySystemComponentFromActorInfo()->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	float ReserveAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentReserveAmmoAttribute(), bFound);
	if (WeaponAS && ReserveAmmo <= 0.f)
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

	// Remove reloading tag
	FGameplayTagContainer Tags;
	Tags.AddTag(ComplyTags::States::State_Reloading);
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);

	// Resume firing if still holding, otherwise end the ability to allow future inputs (as it's instanced per actor)dddd
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