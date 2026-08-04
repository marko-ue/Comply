// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyCrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UComplyCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeCrosshair();
};
