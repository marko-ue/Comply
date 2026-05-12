// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "ThrowableAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UThrowableAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void Throw();
};
