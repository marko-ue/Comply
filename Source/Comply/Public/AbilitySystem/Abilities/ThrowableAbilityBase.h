// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "ThrowableAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UThrowableAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void Throw();
};
