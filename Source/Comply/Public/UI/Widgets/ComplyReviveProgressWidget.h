// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ComplyReviveProgressWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;
/**
 * 
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
