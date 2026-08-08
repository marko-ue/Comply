// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyHUDWidget.generated.h"

class UComplyChargeWidget;
class UComplyAmmoWidget;
class UDamageNumbersWidget;
class UComplyCrosshairWidget;
class UAbilitySystemComponent;
class UComplyHealthBarWidget;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitializeHUD(UAbilitySystemComponent* ASC);
	
	UFUNCTION()
	void TryInitializeWidgets();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyHealthBarWidget> ComplyHealthBarWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyCrosshairWidget> ComplyCrosshairWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyAmmoWidget> ComplyAmmoWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyChargeWidget> ComplyChargeWidget;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;
	
	bool bHealthInitialized;
	bool bAmmoInitialized;
	bool bChargeInitialized;
	bool bWidgetsInitialized;
	
	FTimerHandle RetryTimerHandle;
};
