// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"

void UComplyHealthBarWidget::InitializeHealthBar(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;
	ASC = InASC;
	
	HealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
		UComplyAttributeSet::GetHealthAttribute()
	).AddUObject(this, &UComplyHealthBarWidget::OnHealthChanged);

	MaxHealthChangedHandle = ASC->GetGameplayAttributeValueChangeDelegate(
		UComplyAttributeSet::GetMaxHealthAttribute()
	).AddUObject(this, &UComplyHealthBarWidget::OnMaxHealthChanged);
	
	bool bFound;
	CurrentHealth = ASC->GetGameplayAttributeValue(
		UComplyAttributeSet::GetHealthAttribute(), bFound
	);
	CurrentMaxHealth = ASC->GetGameplayAttributeValue(
		UComplyAttributeSet::GetMaxHealthAttribute(), bFound
	);

	UpdateBar();
	
	// Sync ghost bar to initial health value
	const float InitialPercent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;
	GhostPercent = InitialPercent;
	if (GhostProgressBar) GhostProgressBar->SetPercent(GhostPercent);
}

void UComplyHealthBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!GhostProgressBar) return;
	// Don't interpolate if the ghost bar percent is the same (or less) than the health bar
	if (GhostPercent <= HealthProgressBar->GetPercent()) return;
	
	TimeSinceLastDamage += InDeltaTime;
	// Don't interpolate if not enough time has passed since the ghost bar should start lowering percent
	if (TimeSinceLastDamage < GhostDrainDelay) return;

	// Interpolate the ghost bar percent to the new health percent
	GhostPercent = FMath::FInterpConstantTo(GhostPercent, HealthProgressBar->GetPercent(), InDeltaTime, GhostDrainSpeed);
	GhostProgressBar->SetPercent(GhostPercent);
}

void UComplyHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;
	UpdateBar();
}

void UComplyHealthBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentMaxHealth = Data.NewValue;
	UpdateBar();
}

void UComplyHealthBarWidget::UpdateBar()
{
	if (!HealthProgressBar || !GhostProgressBar || !HealthText) return;

	const float Percent = CurrentMaxHealth > 0.f ? CurrentHealth / CurrentMaxHealth : 0.f;

	// Real bar always snaps immediately
	HealthProgressBar->SetPercent(Percent);

	if (Percent < GhostPercent)
	{
		// Took damage, reset delay, ghost bar holds where it is
		TimeSinceLastDamage = 0.f;
	}
	else
	{
		// Healed, ghost bar snaps up with the real bar, no lag on healing
		GhostPercent = Percent;
		GhostProgressBar->SetPercent(GhostPercent);
	}

	HealthText->SetText(FText::FromString(
		FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, CurrentMaxHealth)
	));
}
