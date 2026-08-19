// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyTeammatePanelWidget.generated.h"

class AComplyPlayerState;
struct FOnAttributeChangeData;
class UAbilitySystemComponent;
class UProgressBar;
class UImage;
class UWidgetAnimation;
class AComplyPlayerCharacter;

UCLASS()
class COMPLY_API UComplyTeammatePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializePanel(UAbilitySystemComponent* InASC, AComplyPlayerCharacter* InCharacter, AComplyPlayerState* InPlayerState);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> TeammatePanelImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LowAmmoImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LowAmmoFlashAnimation;
	
	FORCEINLINE AComplyPlayerState* GetPlayerState() const { return PlayerState; }

private:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnReserveAmmoChanged(const FOnAttributeChangeData& Data);

	FDelegateHandle HealthChangedHandle;
	FDelegateHandle MaxHealthChangedHandle;
	FDelegateHandle ReserveAmmoChangedHandle;

	float CurrentHealth = 0.f;
	float CurrentMaxHealth = 1.f;
	float CachedMaxReserveAmmo = 1.f;

	static constexpr float LowAmmoThreshold = 0.2f;

	void UpdateHealthBar();
	void UpdateLowAmmoIndicator(float NewReserveAmmo);
	
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<AComplyPlayerCharacter>, TObjectPtr<UTexture2D>> ClassPanelMap;

	UPROPERTY()
	TObjectPtr<AComplyPlayerState> PlayerState;
};
