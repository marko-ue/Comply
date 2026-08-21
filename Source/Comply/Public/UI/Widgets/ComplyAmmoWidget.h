// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "ComplyAmmoWidget.generated.h"

struct FOnAttributeChangeData;
class UTextBlock;
class UImage;
class URangedWeaponAbilityBase;

UCLASS()
class COMPLY_API UComplyAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:
	void InitializeAmmo();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
    
private:
	void OnAmmoChanged(const FOnAttributeChangeData& Data);
	void OnReserveAmmoChanged(const FOnAttributeChangeData& Data);
	void RefreshAmmoText() const;
	void RefreshReserveAmmoText() const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BulletImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReserveAmmoText;

	UPROPERTY()
	TObjectPtr<URangedWeaponAbilityBase> ActiveWeapon;

	FGameplayAttribute ActiveAmmoAttribute;
	FGameplayAttribute ActiveReserveAmmoAttribute;

	FDelegateHandle AmmoChangedHandle;
	FDelegateHandle ReserveAmmoChangedHandle;
    
	float CachedAmmo = 0.f;
	float CachedMaxAmmo = 0.f;
	float CachedReserveAmmo = 0.f;
	float CachedMaxReserveAmmo = 0.f;
	
	FTimerHandle AmmoInitRetryHandle;
	void TryInitializeAmmo();
};