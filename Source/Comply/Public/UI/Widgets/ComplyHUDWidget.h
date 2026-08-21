// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyHUDWidget.generated.h"

struct FComplyHUDLayout;
class USizeBox;
class UComplyChatBoxWidget;
class UComplyTeamStatusPanelsWidget;
class UComplyReviveProgressWidget;
class UComplyStatusEffectsWidget;
class UComplyUtilityCooldownsWidget;
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
	void InitializeLayout(const FComplyHUDLayout& Layout);
	
	UFUNCTION()
	void TryInitializeWidgets();
	
	FORCEINLINE UComplyCrosshairWidget* GetCrosshairWidget() const { return ComplyCrosshairWidget; }
	FORCEINLINE UComplyTeamStatusPanelsWidget* GetTeamStatusPanelsWidget() const { return ComplyTeamStatusPanelsWidget; }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyHealthBarWidget> ComplyHealthBarWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyCrosshairWidget> ComplyCrosshairWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyAmmoWidget> ComplyAmmoWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyChargeWidget> ComplyChargeWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyUtilityCooldownsWidget> ComplyUtilityCooldownsWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyStatusEffectsWidget> ComplyStatusEffectsWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyReviveProgressWidget> ComplyReviveProgressWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyTeamStatusPanelsWidget> ComplyTeamStatusPanelsWidget;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> AmmoSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> CooldownSizeBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ChargeSizeBox;
};
