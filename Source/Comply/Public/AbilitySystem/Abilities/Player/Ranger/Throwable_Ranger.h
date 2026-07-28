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
	
public:
	// This will be a scalable float in the future for upgrades
	UPROPERTY(EditAnywhere, Category = "Grenade Settings")
	float ExplosionRadius = 1000.f;

protected:
	virtual void ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual void CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity, FVector SpawnPosition) override;
	
	virtual FGameplayAttribute GetThrowableCurrentChargesAttribute() override;
};
