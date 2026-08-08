// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyUtilityCooldownsWidget.generated.h"

class UTextBlock;
class UUtilityAbilityBase;
class UImage;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyUtilityCooldownsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeUtilityCooldown();
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	void RefreshCooldown() const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UtilityImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownText;

	UPROPERTY()
	TObjectPtr<UUtilityAbilityBase> ActiveUtility;
};
