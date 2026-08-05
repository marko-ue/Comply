// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyEnemyHealthBarWidget.h"

#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "Components/ProgressBar.h"

void UComplyEnemyHealthBarWidget::InitializeHealthBar(UAbilitySystemComponent* InASC)
{
	// Widget starts off hidden
	SetVisibility(ESlateVisibility::Hidden);
	
	ASC = InASC;
	
	HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
		UComplyAttributeSet::GetHealthAttribute()
	).AddUObject(this, &UComplyEnemyHealthBarWidget::OnHealthChanged);
	
	MaxHealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
		UComplyAttributeSet::GetMaxHealthAttribute()
	).AddUObject(this, &UComplyEnemyHealthBarWidget::OnMaxHealthChanged);
	
	bool bFound;
	CurrentHealth = ASC->GetGameplayAttributeValue(
		UComplyAttributeSet::GetHealthAttribute(), bFound
	);
	CurrentMaxHealth = ASC->GetGameplayAttributeValue(
		UComplyAttributeSet::GetMaxHealthAttribute(), bFound
	);
	
	UpdateBar();
}

void UComplyEnemyHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	// The widget becomes visible once enemies take damage
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
	}
	
	CurrentHealth = Data.NewValue;
	UpdateBar();
}

void UComplyEnemyHealthBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	UpdateBar();
}

void UComplyEnemyHealthBarWidget::UpdateBar()
{
	if (!HealthProgressBar) return;

	const float Percent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;

	// Bar snaps to new health immediately
	HealthProgressBar->SetPercent(Percent);
}
