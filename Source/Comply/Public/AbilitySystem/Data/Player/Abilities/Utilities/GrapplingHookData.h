// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GrapplingHookData.generated.h"

class UCurveVector;
/**
 * 
 */
UCLASS()
class COMPLY_API UGrapplingHookData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Grappling Hook Settings")
	float PullDuration = 0.8f;
	
	// Curve for the path (hook swing feel)
	UPROPERTY(EditDefaultsOnly, Category = "Grappling Hook Settings")
	TObjectPtr<UCurveVector> PathOffsetCurve = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grappling Hook Settings")
	float GrappleRange = 3000.f;
};
