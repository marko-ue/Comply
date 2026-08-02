// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemies/EnemyAbilityBase.h"
#include "Attack_Mech.generated.h"

class UMechProjectileData;
class AMechProjectile;
/**
 * 
 */
UCLASS()
class COMPLY_API UAttack_Mech : public UEnemyAbilityBase
{
	GENERATED_BODY()
	
public:
	UAttack_Mech();
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UMechProjectileData> ProjectileData;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void OnAttackAnimationFinished() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AMechProjectile> MechProjectileClass;
};
