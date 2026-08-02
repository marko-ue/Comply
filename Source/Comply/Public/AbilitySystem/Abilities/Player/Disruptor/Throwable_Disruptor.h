// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Disruptor.generated.h"

class UDecoyGrenadeData;
/**
 * 
 */
UCLASS()
class COMPLY_API UThrowable_Disruptor : public UThrowableAbilityBase
{
	GENERATED_BODY()

protected:
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual void CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute() override;
};
