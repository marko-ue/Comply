// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UtilityPreviewBase.generated.h"

class UComplyUtilityData;

UCLASS()
class COMPLY_API AUtilityPreviewBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ACharacter* OwnerCharacter;
	
	virtual void InitPreviewData(ACharacter* OwnerChar, const UComplyUtilityData* InUtilityData);
	
	bool bCanPlace = false;
};
