// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Utility_Disruptor.h"

#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "Actors/AbilityActors/BuffTotem/BuffTotemPreview.h"


void UUtility_Disruptor::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UUtility_Disruptor::ConfirmPlacement()
{
	if (!SpawnedUtilityPreviewActor || !SpawnedUtilityPreviewActor->bCanPlace)
	{
		// Re-create the task so the player can try placing again
		UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
		WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmPlacement);
		WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelPlacement);
		WaitConfirm->ReadyForActivation();
		return;
	}
	
	Super::ConfirmPlacement();
}

void UUtility_Disruptor::TraceAndSpawn()
{
	// A server RPC is used to handle spawning the buff totem
	if (UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->Server_PlaceBuffTotem(GetCurrentAbilitySpecHandle(), CachedPlaceLocation, UtilityLifetime);
	}
	
	// Automatically equip the primary ability once the buff totem is thrown, as the player should not be able to equip the buff totem while it's on cooldown
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(
		FGameplayTagContainer(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary)
	);
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
