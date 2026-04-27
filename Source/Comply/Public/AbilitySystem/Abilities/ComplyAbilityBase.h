// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ComplyAbilityBase.generated.h"


class UAbilityTask_PlayMontageAndWait;

UCLASS(Abstract)
class COMPLY_API UComplyAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void OnMontageCompleted();

	UFUNCTION()
	virtual void OnMontageCancelled();
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayActivationMontageTask;
};
