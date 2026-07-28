// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Enforcer.generated.h"

class UAbilityTask_WaitConfirmCancel;
class ADeployableTurret;
class ADeployableTurretPreview;
/**
 *
 */
UCLASS()
class COMPLY_API UThrowable_Enforcer : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	UFUNCTION()
	void ConfirmPlace();
	
	UPROPERTY()
	TObjectPtr<ADeployableTurretPreview> SpawnedTurretPreviewActor = nullptr;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Turret Settings")
	float TurretLifetime = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<ADeployableTurret> TurretActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> TurretPreviewActorClass;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void SpawnPreview() override;

	void PlaceOnServer(FVector LaunchVelocity, FVector SpawnPosition);
	
	UFUNCTION()
	void CancelThrow();
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute() override;
	
private:
	void PlaceTurret();
	
	UFUNCTION()
	void PlayPlaceTurretAnimation();
	
	UFUNCTION()
	void PlaceTurretAnimationInterrupted();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> PlaceTurretMontage;
	
	UPROPERTY()
	UAbilityTask_WaitConfirmCancel* WaitConfirm;
	
	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ApplicationTag);
};
