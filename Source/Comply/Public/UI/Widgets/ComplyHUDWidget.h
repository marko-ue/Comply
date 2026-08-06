// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyHUDWidget.generated.h"

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

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyHealthBarWidget> ComplyHealthBarWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UComplyCrosshairWidget> ComplyCrosshairWidget;
};
