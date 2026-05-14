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
	
public:
	virtual void ConfirmThrow();
	
protected:
	virtual void Throw();
	
	
	UPROPERTY(EditAnywhere)
	float ThrowSpeed = 1000.f;
};
