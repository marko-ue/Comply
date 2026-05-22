// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Ranger.generated.h"

class APlasmaGrenade;
class APlasmaGrenadePreview;
/**
 * TODO: Make damage a scalable float for upgrades
 */
UCLASS()
class COMPLY_API UThrowable_Ranger : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	virtual bool CommitAbilityCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	UFUNCTION()
	virtual void ConfirmThrow() override;
	
	UPROPERTY()
	TObjectPtr<APlasmaGrenadePreview> SpawnedGrenadePreviewActor;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CostEffectClass;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<APlasmaGrenade> GrenadeActorClass;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void SpawnPreview() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> GrenadePreviewActorClass;
};
