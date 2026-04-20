// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HitscanTargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitscanTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);
/**
 * This ability task will be used for sending hitscan target data to the server.
 * This is needed in order to make the CauseDamage function run on the server, and therefore firing the execution calculation
 */
UCLASS()
class COMPLY_API UHitscanTargetData : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	void SendHitscanTargetData(float TraceDistance);
	
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "HitscanTargetData", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UHitscanTargetData* CreateHitScanData(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable)
	FHitscanTargetDataSignature ValidData;
	
private:
	virtual void Activate() override;
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
