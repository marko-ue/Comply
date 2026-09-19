// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"

// UE
#include "GameplayTagContainer.h"

#include "ComplyReviveProgressWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

/**
 * Widget that displays the revive progress (how long until revive finishes).
 */
UCLASS()
class COMPLY_API UComplyReviveProgressWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeReviveProgress(UAbilitySystemComponent* InASC);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void OnReviveTagChanged(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ReviveProgressBar;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	float ReviveDuration = 0.f;
	bool bIsReviving = false;
};
