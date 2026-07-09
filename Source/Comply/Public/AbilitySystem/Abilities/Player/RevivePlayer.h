// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RevivePlayer.generated.h"

class AComplyPlayerCharacter;
/**
 * 
 */
UCLASS()
class COMPLY_API URevivePlayer : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UFUNCTION()
	void OnHoldComplete();
	
	UPROPERTY()
	TObjectPtr<AComplyPlayerCharacter> Reviver;

	UPROPERTY()
	TObjectPtr<AComplyPlayerCharacter> TargetPlayer;
	
	UPROPERTY(EditAnywhere)
	float ReviveTime = 5.f;
	
	UPROPERTY(EditAnywhere)
	float ReviveSphereTraceRadius = 100.f;
};
