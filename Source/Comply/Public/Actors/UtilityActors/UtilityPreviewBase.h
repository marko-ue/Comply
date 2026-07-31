// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UtilityPreviewBase.generated.h"

UCLASS()
class COMPLY_API AUtilityPreviewBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ACharacter* OwnerCharacter;
	
	void InitPreviewData(ACharacter* OwnerChar);
	
	bool bCanPlace = false;
};
