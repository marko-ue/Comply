#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"

UHitscanTargetData* UHitscanTargetData::CreateHitScanData(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<UHitscanTargetData>(OwningAbility);
}

void UHitscanTargetData::Activate()
{
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendHitscanTargetData(Cast<URangedWeaponAbilityBase>(Ability)->TraceDistance);
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
			.AddUObject(this, &UHitscanTargetData::OnTargetDataReplicatedCallback);

		const bool bCalledDelegate =
			AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UHitscanTargetData::SendHitscanTargetData(float TraceDistance)
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	URangedWeaponAbilityBase* RangedWeaponBase = Cast<URangedWeaponAbilityBase>(Ability);

	if (RangedWeaponBase->DoesWeaponUseCrosshairTrace())
	{
		FHitResult Hit;
		bool bPassedThroughShield = false;

		RangedWeaponBase->TraceToCrosshair(Hit, 10000.f, bPassedThroughShield);

		FComplyGameplayAbilityTargetData_SingleHit* Data =
			new FComplyGameplayAbilityTargetData_SingleHit();

		Data->HitResult = Hit;
		Data->bPassedThroughShield = bPassedThroughShield;

		DataHandle.Add(Data);
	}
	else
	{
		UPrimary_Disruptor* Primary = Cast<UPrimary_Disruptor>(Ability);
		if (Primary)
		{
			TArray<FHitResult> Hits;
			bool bPassedThroughShield = false;

			Primary->PerformShotgunTraces(Hits, Primary->NumberOfPellets, 10000.f, bPassedThroughShield);

			for (const FHitResult& Hit : Hits)
			{
				FComplyGameplayAbilityTargetData_SingleHit* Data =
					new FComplyGameplayAbilityTargetData_SingleHit();

				Data->HitResult = Hit;
				Data->bPassedThroughShield = bPassedThroughShield;

				DataHandle.Add(Data);
			}
		}
	}

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UHitscanTargetData::OnTargetDataReplicatedCallback(
	const FGameplayAbilityTargetDataHandle& InDataHandle,
	FGameplayTag ActivationTag) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(InDataHandle);
	}
}