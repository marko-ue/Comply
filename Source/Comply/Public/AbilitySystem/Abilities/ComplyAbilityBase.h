// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ComplyAbilityBase.generated.h"



UCLASS(Abstract)
class COMPLY_API UComplyAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	
protected:
	UFUNCTION()
	virtual void OnMontageCompleted();

	UFUNCTION()
	virtual void OnMontageCancelled();
	
private:

};
