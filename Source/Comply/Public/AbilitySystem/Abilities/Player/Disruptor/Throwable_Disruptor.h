// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Disruptor.generated.h"

class IWeaponInterface;
class ADecoyGrenadePreview;
class ADecoyGrenade;
/**
 * 
 */
UCLASS()
class COMPLY_API UThrowable_Disruptor : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
public:
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	virtual bool CommitAbilityCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	UFUNCTION()
	virtual void ConfirmThrow() override;
	
	void EquipWeaponBasedOnCharges(IWeaponInterface* WeaponOwner, UAbilitySystemComponent* ASC);
	
	UPROPERTY()
	TObjectPtr<ADecoyGrenadePreview> SpawnedDecoyGrenadePreviewActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> CostEffectClass;
	 
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float PullRadius = 1000.f;

	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float DecoyGrenadeLifetime = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<ADecoyGrenade> DecoyGrenadeActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> DecoyGrenadePreviewActorClass;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void SpawnPreview() override;
	
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ThrowDecoyMontage;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PrepareDecoyMontageTask;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> NoThrowablesEffectClass;
	
	UFUNCTION()
	void OnThrowMontageCompleted();
	
};
