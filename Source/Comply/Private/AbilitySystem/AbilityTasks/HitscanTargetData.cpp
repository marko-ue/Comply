// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"


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
	URangedWeaponAbilityBase* RangedWeaponBase = Cast<URangedWeaponAbilityBase>(Ability);
	
	if (RangedWeaponBase->DoesWeaponUseCrosshairTrace())
	{
		FHitResult CrosshairHitscanHit;
		RangedWeaponBase->TraceToCrosshair(CrosshairHitscanHit, 10000.f, bPassedThroughShield);
		FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
		Data->HitResult = CrosshairHitscanHit;
		DataHandle.Add(Data);
	}
	else // If weapon does not use a simple crosshair trace (shotgun)
	{
		UPrimary_Disruptor* PrimaryDisruptor = Cast<UPrimary_Disruptor>(Ability);
		if (PrimaryDisruptor)
		{
			TArray<FHitResult> ShotgunHitscanHits;
			PrimaryDisruptor->PerformShotgunTraces(ShotgunHitscanHits, PrimaryDisruptor->NumberOfPellets, 10000.f, bPassedThroughShield);

			// Send data for each shotgun hit, damage is applied per hit individually
			for (const FHitResult& Hit : ShotgunHitscanHits)
			{
				FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
				Data->HitResult = Hit;
				DataHandle.Add(Data);
			}
		}
	}

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
	
	UE_LOG(LogTemp, Warning,
	TEXT("ActivationKey %s"),
	*GetActivationPredictionKey().ToString());
}

// Consumes the sent data when received
void UHitscanTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& InDataHandle,
	FGameplayTag ActivationTag) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(InDataHandle);
	}
}
