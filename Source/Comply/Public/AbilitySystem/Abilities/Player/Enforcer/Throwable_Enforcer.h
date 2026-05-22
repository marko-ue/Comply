// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Enforcer.generated.h"

class ADeployableTurret;
class ADeployableTurretPreview;
/**
 * TODO: Make damage a scalable float for upgrades
 */
UCLASS()
class COMPLY_API UThrowable_Enforcer : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	UFUNCTION()
	virtual void ConfirmThrow() override;
	
	UPROPERTY()
	TObjectPtr<ADeployableTurretPreview> SpawnedTurretPreviewActor = nullptr;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere)
	float TurretLifetime = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADeployableTurret> TurretActorClass;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	void SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> TurretPreviewActorClass;
	
	virtual void Throw() override;
	
	UFUNCTION()
	virtual void CancelThrow() override;
	
private:
	void PlaceTurret();
};
