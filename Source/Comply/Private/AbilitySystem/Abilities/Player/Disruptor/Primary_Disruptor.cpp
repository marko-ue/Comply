// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"

#include "Character/ComplyPlayerCharacter.h"


void UPrimary_Disruptor::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	// TODO: Apply cosmetic effects (muzzle flash, tracer, sound) that do no damage to make the shot feel responsive
	
	Fire();
}

bool UPrimary_Disruptor::Fire()
{
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return false;

	URangedWeaponAbilityBase* Weapon = Character->GetEquippedPrimaryWeapon();
	if (!Weapon) return false;

	bool bFound = false;
	float CurrentAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(
		Weapon->GetCurrentAmmoAttribute(), bFound);

	bool bIsReloading = GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(ComplyTags::States::State_Reloading);

	// If attempting to activate at 0 ammo while reloading, the player will not be able to
	// This prevents players from continuously interrupting reload when they don't even have a bullet to fire
	// Still allows the activation of the Fire function in the base class which handles automatically reloading
	// if firing on 0 ammo in mag when not previously reloading
	if (!bFound || (CurrentAmmo <= 0.f && bIsReloading))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}

	if (!Super::Fire()) return false;

	PlayAnimationBasedOnState();
	return true;
}
