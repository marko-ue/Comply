// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UtilityAbilityBase.h"
#include "Utility_Ranger.generated.h"

class AShieldDomePreview;

/**
 * 
 */
UCLASS()
class COMPLY_API UUtility_Ranger : public UUtilityAbilityBase
{
	GENERATED_BODY()

public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> ShieldActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AShieldDomePreview> ShieldPreviewActorClass;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	// The EndAbility function needs to be overridden in this case because it's likely that otherwise it was being ended before the actor actually got destroyed on the client
	// The function itself now handles destroying the shield actor, ensuring it always happens when the ability is ended
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void Use() override;
	
private:
	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnedShieldActor = nullptr;
	
	UPROPERTY()
	TObjectPtr<AShieldDomePreview> SpawnedShieldPreviewActor = nullptr;
	
	UFUNCTION()
	void TraceAndSpawnShield();
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float ShieldLifetime = 10.f;
	
	void SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo);
	
	UFUNCTION()
	void ConfirmPlacement();
	
	UFUNCTION()
	void CancelPlacement();
	
	FTimerHandle OnShieldExpiredTimerHandle;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> PlaceShieldMontage;
};
