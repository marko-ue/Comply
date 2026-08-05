// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyEnemyHealthBarWidget.generated.h"

struct FOnAttributeChangeData;
class UProgressBar;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void InitializeHealthBar(UAbilitySystemComponent* InASC);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
	
private:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	
	FDelegateHandle HealthChangedHandle;
	FDelegateHandle MaxHealthChangedHandle;
	
	float CurrentHealth = 0.f;
	float CurrentMaxHealth = 1.f;
	
	void UpdateBar();
};
