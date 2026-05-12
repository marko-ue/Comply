// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Ranger.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UThrowable_Ranger : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void Throw() override;
};
