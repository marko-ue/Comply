#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HitscanTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitscanTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

UCLASS()
class COMPLY_API UHitscanTargetData : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
	meta=(DisplayName="HitscanTargetData", HidePin="OwningAbility", DefaultToSelf="OwningAbility", BlueprintInternalUseOnly="true"))
	static UHitscanTargetData* CreateHitScanData(UGameplayAbility* OwningAbility);

	void SendHitscanTargetData(float TraceDistance);

	UPROPERTY(BlueprintAssignable)
	FHitscanTargetDataSignature ValidData;

protected:
	virtual void Activate() override;

private:
	void OnTargetDataReplicatedCallback(
		const FGameplayAbilityTargetDataHandle& InDataHandle,
		FGameplayTag ActivationTag) const;

	FGameplayAbilityTargetDataHandle DataHandle;
};