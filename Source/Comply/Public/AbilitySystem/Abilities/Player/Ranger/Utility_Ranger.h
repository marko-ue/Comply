// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Ranger.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UUtility_Ranger : public UUtilityAbilityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ShieldActorClass;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// The EndAbility function needs to be overridden in this case because it's likely that otherwise it was being ended before the actor actually got destroyed on the client
	// The function itself now handles destroying the shield actor, ensuring it always happens when the ability is ended
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void Use() override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnedActor = nullptr;
	
	void TraceAndSpawnShield();
	
	UFUNCTION()
	void OnShieldExpired();
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere)
	float ShieldLifetime = 10.f;
	
};
