// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Interface/Player/WeaponInterface.h"
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
	
	void EquipWeaponBasedOnCharges(IWeaponInterface* WeaponOwner, UAbilitySystemComponent* ASC);

	UPROPERTY()
	TObjectPtr<APlasmaGrenadePreview> SpawnedGrenadePreviewActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> CostEffectClass;
	
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float ExplosionRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<APlasmaGrenade> GrenadeActorClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> GrenadePreviewActorClass;

	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void SpawnPreview() override;
	
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ThrowGrenadeMontage;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PrepareGrenadeMontageTask;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> NoThrowablesEffectClass;
	
	UFUNCTION()
	void OnThrowMontageCompleted();
};
