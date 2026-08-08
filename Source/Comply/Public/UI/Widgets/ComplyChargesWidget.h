// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "ComplyChargesWidget.generated.h"


class UThrowableAbilityBase;
struct FOnAttributeChangeData;
class UTextBlock;
class UImage;
class UTexture2D;
 
UCLASS()
class COMPLY_API UComplyChargeWidget : public UUserWidget
{
	GENERATED_BODY()
 
public:
	void InitializeCharge();
 
private:
	void OnChargeChanged(const FOnAttributeChangeData& Data);
	void RefreshChargeText() const;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ChargeImage;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ChargeText;
	
	UPROPERTY()
	TObjectPtr<UThrowableAbilityBase> ActiveThrowable;
 
	FGameplayAttribute ChargeAttribute;
	FGameplayAttribute MaxChargeAttribute;
 
	FDelegateHandle ChargeChangedHandle;
 
	float CachedCharge    = 0.f;
	float CachedMaxCharge = 0.f;
};
