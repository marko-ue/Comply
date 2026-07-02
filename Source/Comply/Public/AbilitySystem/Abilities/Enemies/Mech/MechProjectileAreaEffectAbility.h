// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemies/EnemyAbilityBase.h"
#include "MechProjectileAreaEffectAbility.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UMechProjectileAreaEffectAbility : public UEnemyAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
