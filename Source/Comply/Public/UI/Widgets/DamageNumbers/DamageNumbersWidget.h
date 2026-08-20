// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumbersWidget.generated.h"

class AComplyCharacterBase;
class AComplyEnemyCharacter;
class UCanvasPanel;
class UDamageNumberEntryWidget;
struct FOnAttributeChangeData;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class COMPLY_API UDamageNumbersWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void ShowDamageNumber(const float DamageAmount, const FVector& WorldPos, const FLinearColor Color);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvas;
	
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<AComplyCharacterBase> DamagedCharacter;

	FDelegateHandle HealthChangedHandle;

	float CurrentHealth = 0.f;
	
	UPROPERTY()
	TArray<UDamageNumberEntryWidget*> Pool;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageNumberEntryWidget> EntryWidgetClass;
	
	UDamageNumberEntryWidget* GetPooledEntry();
};
