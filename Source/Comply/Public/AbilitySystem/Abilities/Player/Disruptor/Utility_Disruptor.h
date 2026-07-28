// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Disruptor.generated.h"

class ABuffTotem;
class ABuffTotemPreview;
/**
 * 
 */
UCLASS()
class COMPLY_API UUtility_Disruptor : public UUtilityAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<ABuffTotem> BuffTotemActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<ABuffTotemPreview> BuffTotemPreviewActorClass;
	
	UPROPERTY()
	TObjectPtr<ABuffTotemPreview> SpawnedBuffTotemPreviewActor = nullptr;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void Use() override;
	
private:
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Totem Settings")
	float BuffTotemLifetime = 20.f;
	
	void SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo);
	
	UFUNCTION()
	void TraceAndSpawnBuffTotem();
	
	UFUNCTION()
	void ConfirmPlacement();
	
	UFUNCTION()
	void CancelPlacement();
	
	FTimerHandle OnBeaconExpiredTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> PlaceBuffTotemMontage;
};
