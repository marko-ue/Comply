// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/DamageAbilityBase.h"
#include "MeleeAttack_Mech.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UMeleeAttack_Mech : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	UMeleeAttack_Mech();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UFUNCTION()
	void OnAttackAnimationFinished();
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
};
