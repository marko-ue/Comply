// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyGrenadeData.h"
#include "DecoyGrenadeData.generated.h"

class UNiagaraSystem;
class USoundCue;
/**
 * 
 */
UCLASS()
class COMPLY_API UDecoyGrenadeData : public UComplyGrenadeData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Settings")
	float PullRadius = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grenade Settings")
	float GrenadeLifetime = 20.f;
};
