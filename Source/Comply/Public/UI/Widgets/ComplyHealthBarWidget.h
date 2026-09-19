// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"

#include "ComplyHealthBarWidget.generated.h"

/**
 * 
 */

struct FOnAttributeChangeData;
class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;

UCLASS()
class COMPLY_API UComplyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitializeHealthBar(UAbilitySystemComponent* InASC);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> GhostProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ghost Health Bar")
	float GhostDrainDelay = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Ghost Health Bar")
	float GhostDrainSpeed = 0.35f;

private:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	FDelegateHandle HealthChangedHandle;
	FDelegateHandle MaxHealthChangedHandle;

	float CurrentHealth = 0.f;
	float CurrentMaxHealth = 1.f;
	
	float GhostPercent = 1.f;
	float TimeSinceLastDamage = 0.f;

	void UpdateBar();
};
