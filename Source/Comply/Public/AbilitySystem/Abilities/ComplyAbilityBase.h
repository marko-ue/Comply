// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ComplyAbilityBase.generated.h"

class UComplyAbilityData;
class UAbilityTask_PlayMontageAndWait;

UCLASS(Abstract)
class COMPLY_API UComplyAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyAbilityData> AbilityData;
	
	virtual UGameplayEffect* GetCostGameplayEffect() const override;
	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;
	
protected:
	UFUNCTION()
	virtual void OnMontageCompleted();

	UFUNCTION()
	virtual void OnMontageCancelled();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayActivationMontageTask;
};
