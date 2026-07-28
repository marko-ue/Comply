// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "ThrowableAbilityBase.generated.h"

class UComplyAbilitySystemComponent;
class IWeaponInterface;
class AThrowablePreviewBase;
/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UThrowableAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void ConfirmThrow();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Types")
	bool bConfirmOnRelease = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Types")
	bool bSpawnPreviewOnActivate = true;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Settings")
	float ThrowSpeed = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> CostEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> ThrowableActorClass;

	virtual void EquipWeaponBasedOnCharges(IWeaponInterface* WeaponOwner, UAbilitySystemComponent* ASC);
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	virtual bool CommitAbilityCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) override;
	
	virtual void SpawnPreview();
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition);
	
	UFUNCTION()
	virtual void OnThrowMontageCompleted();
	
	virtual void CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition);
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute();
	
	void SafeRemoveThrowingTag() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> NoThrowablesEffectClass;
	
	UPROPERTY()
	TObjectPtr<AThrowablePreviewBase> SpawnedThrowablePreviewActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actors")
	TSubclassOf<AActor> ThrowablePreviewActorClass;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ThrowMontage;
	
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PrepareThrowMontageTask;
};
