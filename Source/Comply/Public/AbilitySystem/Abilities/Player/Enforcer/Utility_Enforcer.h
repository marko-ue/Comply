// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Enforcer.generated.h"

class AEnforcerCharacter;
class UCableComponent;
class AComplyPlayerCharacter;
class UCurveVector;
/**
 * 
 */
UCLASS()
class COMPLY_API UUtility_Enforcer : public UUtilityAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grappling Hook Settings")
	float PullDuration = 0.8f;
	
	// Curve for the path (hook swing feel)
	UPROPERTY(EditDefaultsOnly, Category = "Grappling Hook Settings")
	UCurveVector* PathOffsetCurve = nullptr;
	
private:
	bool PerformGrappleTrace(FHitResult& OutHitResult, float GrappleRange = 3000.f) const;
	
	UFUNCTION()
	void OnPullReachedDestination();

	UFUNCTION()
	void OnPullTimedOut();

	void FinishGrapple();
	
	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ApplicationTag);

	UPROPERTY()
	TObjectPtr<AEnforcerCharacter> Player;

	UPROPERTY()
	TObjectPtr<UCableComponent> Cable;
	
	FVector GrappleWorldTarget;
	
	FTimerHandle CableUpdateTimer;
};
