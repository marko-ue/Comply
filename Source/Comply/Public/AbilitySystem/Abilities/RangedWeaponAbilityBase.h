// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "DamageAbilityBase.h"
#include "Camera/CameraComponent.h"
#include "RangedWeaponAbilityBase.generated.h"

class UCameraComponent;
/**
 * 
 */
UCLASS(Abstract)
class COMPLY_API URangedWeaponAbilityBase : public UDamageAbilityBase
{
	GENERATED_BODY()
	
public:
	void TraceToCrosshair(FHitResult& TraceHitResult, float TraceDistance);
	
	UPROPERTY(EditDefaultsOnly)
	float TraceDistance = 10000.f;
protected:
	UPROPERTY(EditDefaultsOnly)
	FVector Start;
	
	UPROPERTY(EditDefaultsOnly)
	FVector End;
	
	/*
	 * Cached variables used to end the ability
	 */
	FGameplayAbilitySpecHandle CachedHandle;
	const FGameplayAbilityActorInfo* CachedActorInfo;
	FGameplayAbilityActivationInfo CachedActivationInfo;
};
