// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyCrosshairWidget.generated.h"

class UImage;
/**
 * 
 */

UCLASS()
class COMPLY_API UComplyCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeCrosshair();
	void SetCrosshairTexture(UTexture2D* CrosshairTexture);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage;
};
