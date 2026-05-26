// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyAbilityBase.h"
#include "UtilityAbilityBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UUtilityAbilityBase : public UComplyAbilityBase
{
	GENERATED_BODY()
	
protected:
	virtual void Use();
};
