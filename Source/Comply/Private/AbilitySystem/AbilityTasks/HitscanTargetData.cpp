// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"


UHitscanTargetData* UHitscanTargetData::CreateHitScanData(UGameplayAbility* OwningAbility)
{
	UHitscanTargetData* Obj = NewAbilityTask<UHitscanTargetData>(OwningAbility);
	return Obj;
}

void UHitscanTargetData::Activate()
{
	// If the player activating this ability task is a client, only then send hit scan target data to the server
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendHitscanTargetData(Cast<URangedWeaponAbilityBase>(Ability)->TraceDistance);
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UHitscanTargetData::OnTargetDataReplicatedCallback);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

// Calls the trace function and sends the result to the server
void UHitscanTargetData::SendHitscanTargetData(float TraceDistance)
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
    
	// A member boolean variable is created that will be populated with the result of if the hit was through a shield or not
	FHitResult HitscanHit;
	Cast<URangedWeaponAbilityBase>(Ability)->TraceToCrosshair(HitscanHit, 10000.f, bPassedThroughShield);
    
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitscanHit;
	DataHandle.Add(Data);
    
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
    
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

// Consumes the sent data when received
void UHitscanTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
