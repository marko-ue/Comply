// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Enforcer.generated.h"

class UGrapplingHookData;
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
	
public:
	UPROPERTY(EditdefaultsOnly, Category = "Data")
	TObjectPtr<UGrapplingHookData> GrapplingHookData;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	bool PerformGrappleTrace(FHitResult& OutHitResult, const float GrappleRange) const;
	
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
