// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/EquipAbilities/EquipPrimaryAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/ComplyTags.h"
#include "Interface/Player/WeaponInterface.h"


UEquipPrimaryAbility::UEquipPrimaryAbility()
{
	FGameplayTagContainer Tags;
	Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary);
	SetAssetTags(Tags);
}

void UEquipPrimaryAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	TagsToBlock.RemoveTag(ComplyTags::ComplyAbilities::Primary);
	ASC->BlockAbilitiesWithTags(TagsToBlock);
	
	// Get the relevant weapon mesh from a weapon slot and set that weapon's mesh to be the new mesh
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar))
	{
		WeaponOwner->OnWeaponEquipped(EWeaponSlot::Primary);
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

