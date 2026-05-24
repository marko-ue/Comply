// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ComplyAbilitySystemComponent.generated.h"


class ADeployableTurret;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMPLY_API UComplyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UComplyAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Server, Reliable)
	void Server_PlaceTurret(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation);
	
	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation, FVector InLaunchVelocity);
	
	UFUNCTION(Server, Reliable)
	void Server_PlaceBeacon(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, float BeaconLifetime);
	
	UFUNCTION(Server, Reliable)
	void Server_ThrowDecoyGrenade(FGameplayAbilitySpecHandle AbilityHandle, FVector SpawnLocation, FRotator SpawnRotation, FVector InLaunchVelocity);
};
