// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "UtilityAbilityBase.generated.h"

class UComplyUtilityData;
class AUtilityPreviewBase;
/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UUtilityAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyUtilityData> UtilityData;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void SpawnPreview();
	
	UFUNCTION()
	virtual void ConfirmPlacement();
	
	UFUNCTION()
	virtual void CancelPlacement();
	
	UFUNCTION()
	virtual void TraceAndSpawn();
	
	UPROPERTY()
	TObjectPtr<AUtilityPreviewBase> SpawnedUtilityPreviewActor;
	
	FVector CachedPlaceLocation;
	
private:
	// Move to checking if base utility data exists
	UPROPERTY(EditDefaultsOnly, Category = "Ability Settings")
	bool bDoesAbilitySpawnActor = true;
};
