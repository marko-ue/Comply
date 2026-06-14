// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "ThrowableAbilityBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API UThrowableAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	virtual void ConfirmThrow();
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Properties|Types")
	bool bConfirmOnRelease = true;
	
	UPROPERTY(EditAnywhere, Category = "Ability Properties")
	float ThrowSpeed = 1000.f;
	
protected:
	virtual void SpawnPreview();
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition);
	virtual void CancelThrow();
	void SafeRemoveThrowingTag();
};
