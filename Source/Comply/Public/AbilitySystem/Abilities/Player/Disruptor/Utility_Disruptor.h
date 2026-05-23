// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Disruptor.generated.h"

class AConfusionBeacon;
class AConfusionBeaconPreview;
/**
 * TODO: Make enemies attack each other. This will be done after enemy UI is finished, after enemies are able to attack
 */
UCLASS()
class COMPLY_API UUtility_Disruptor : public UUtilityAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AConfusionBeacon> BeaconActorClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AConfusionBeaconPreview> BeaconPreviewActorClass;
	
	UPROPERTY()
	TObjectPtr<AConfusionBeaconPreview> SpawnedBeaconPreviewActor = nullptr;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void Use() override;
	
private:
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere)
	float BeaconLifetime = 20.f;
	
	void SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo);
	
	void TraceAndSpawnBeacon();
	
	UFUNCTION()
	void ConfirmPlacement();
	
	UFUNCTION()
	void CancelPlacement();
	
	FTimerHandle OnBeaconExpiredTimerHandle;
};
