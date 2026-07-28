// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Disruptor.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UThrowable_Disruptor : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
public:
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Grenade Settings")
	float PullRadius = 1000.f;

	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Grenade Settings")
	float DecoyGrenadeLifetime = 20.f;
	
protected:
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual void CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute() override;
};
