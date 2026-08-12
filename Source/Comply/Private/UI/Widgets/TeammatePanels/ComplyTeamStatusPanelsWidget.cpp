// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/TeammatePanels/ComplyTeamStatusPanelsWidget.h"

#include "Character/ComplyPlayerCharacter.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "UI/Widgets/TeammatePanels/ComplyTeammatePanelWidget.h"

void UComplyTeamStatusPanelsWidget::InitializeTeamStatusPanels()
{
    if (!GetWorld()) return;

    if (PanelsVerticalBox)
    {
        PanelsVerticalBox->ClearChildren();
        TeammateWidgets.Empty();
    }

    const APlayerController* LocalPC = GetOwningPlayer();
    if (!LocalPC) return;

    AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
    if (!GS)
    {
        FTimerHandle RetryHandle;
        GetWorld()->GetTimerManager().SetTimer(RetryHandle, this,
            &UComplyTeamStatusPanelsWidget::InitializeTeamStatusPanels, 0.5f, false);
        return;
    }

    for (APlayerState* PS : GS->PlayerArray)
    {
        if (PS == LocalPC->PlayerState) continue;

        AComplyPlayerState* ComplyPS = Cast<AComplyPlayerState>(PS);
        
        AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(PS->GetPawn());
        
        UAbilitySystemComponent* ASC = ComplyPS->GetAbilitySystemComponent();

        UComplyTeammatePanelWidget* Panel = CreateWidget<UComplyTeammatePanelWidget>(GetOwningPlayer(), TeammatePanelWidgetClass);
        
        if (!ComplyPS || !Character || !ASC || !Panel ) continue;

        Panel->InitializePanel(ASC, Character, ComplyPS);

        if (UVerticalBoxSlot* BoxSlot = PanelsVerticalBox->AddChildToVerticalBox(Panel))
        {
            BoxSlot->SetPadding(FMargin(0.f));
        }

        TeammateWidgets.Add(Panel);
    }
}
