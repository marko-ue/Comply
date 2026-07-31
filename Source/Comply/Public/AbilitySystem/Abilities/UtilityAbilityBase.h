// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "UtilityAbilityBase.generated.h"

class AUtilityPreviewBase;
/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UUtilityAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> UtilityActorClass;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Totem Settings")
	float UtilityLifetime = 20.f;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AUtilityPreviewBase> UtilityPreviewActorClass;
	
	UPROPERTY()
	TObjectPtr<AUtilityPreviewBase> SpawnedUtilityPreviewActor;
	
	UPROPERTY(EditAnywhere, Category = "Animations")
	TObjectPtr<UAnimMontage> PlaceUtilityMontage;
	
	FVector CachedPlaceLocation;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Ability Settings")
	bool bDoesAbilitySpawnActor = true;
};
