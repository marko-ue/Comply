// Copyright © 2026 Marko. All rights reserved.

#include "UI/Widgets/ComplyReviveProgressWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/ProgressBar.h"

void UComplyReviveProgressWidget::InitializeReviveProgress(UAbilitySystemComponent* InASC)
{
    ASC = InASC;

    if (ASC)
    {
        ASC->RegisterGameplayTagEvent(ComplyTags::States::State_Reviving, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &UComplyReviveProgressWidget::OnReviveTagChanged);

        SetRenderOpacity(0.f);
    }
}

void UComplyReviveProgressWidget::OnReviveTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        if (const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(ASC->GetAvatarActor()))
        {
            checkf(Character->PlayerData, TEXT("PlayerData not set on %s"), *Character->GetName());
            ReviveDuration = Character->PlayerData->ReviveMontage->GetPlayLength();
        }

        ReviveProgressBar->SetPercent(0.f);
        ReviveProgressBar->SetFillColorAndOpacity(FLinearColor(1.f, 0.75f, 0.f, 1.f));
        SetRenderOpacity(1.f);
        bIsReviving = true;
    }
    else
    {
        bIsReviving = false;
        SetRenderOpacity(0.f);
        ReviveProgressBar->SetPercent(0.f);
    }
}

void UComplyReviveProgressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bIsReviving || ReviveDuration <= 0.f) return;

    const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(ASC->GetAvatarActor());
    if (!Character) return;

    const UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    const float Position = AnimInstance->Montage_GetPosition(Character->PlayerData->ReviveMontage);
    ReviveProgressBar->SetPercent(FMath::Clamp(Position / ReviveDuration, 0.f, 1.f));
}