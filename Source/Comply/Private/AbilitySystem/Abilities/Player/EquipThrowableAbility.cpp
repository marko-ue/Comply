// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/EquipThrowableAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"

void UEquipThrowableAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	// Reset all block counts first
	FGameplayTagContainer AllWeaponTags;
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Primary);
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Utility);
	AllWeaponTags.AddTag(ComplyTags::ComplyAbilities::Throwable);
	ASC->UnBlockAbilitiesWithTags(AllWeaponTags);

	// Cancel everything, then block all except this ability
	ASC->CancelAbilities(&AllWeaponTags, nullptr, this);

	// Unblock the relevant ability related to this equip ability
	FGameplayTagContainer TagsToBlock = AllWeaponTags;
	TagsToBlock.RemoveTag(ComplyTags::ComplyAbilities::Throwable);
	ASC->BlockAbilitiesWithTags(TagsToBlock);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
