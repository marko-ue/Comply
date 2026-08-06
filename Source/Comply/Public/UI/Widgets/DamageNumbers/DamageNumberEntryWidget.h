// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberEntryWidget.generated.h"

class UTextBlock;
class UDamageNumbersWidget;
/**
 * 
 */
UCLASS()
class COMPLY_API UDamageNumberEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	bool bActive = false;
	
	void Activate(const int32 DamageAmount, const FVector& InWorldPos);
	
	UPROPERTY()
	TObjectPtr<UDamageNumbersWidget> OwningPool;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
protected:
	virtual void NativeConstruct();
	
private:
	UFUNCTION()
	void ReturnToPool();  // Called at end of animation
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FloatUpAnim;
	
	FVector WorldPosition;
};
