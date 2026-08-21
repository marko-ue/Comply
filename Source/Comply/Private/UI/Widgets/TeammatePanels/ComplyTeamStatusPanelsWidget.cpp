// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/TeammatePanels/ComplyTeamStatusPanelsWidget.h"

#include "Character/ComplyPlayerCharacter.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "Framework/PlayerState/ComplyPlayerState.h"
#include "UI/Widgets/TeammatePanels/ComplyTeammatePanelWidget.h"


void UComplyTeamStatusPanelsWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
    if (GS)
    {
        GS->OnVoteKickResolved.AddDynamic(this, &UComplyTeamStatusPanelsWidget::OnVoteKickResolved);
    }
    
    // Start polling until all players have panels
    StartPanelRetryTimer();
}

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

void UComplyTeamStatusPanelsWidget::StartPanelRetryTimer()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;
    if (PC->GetWorldTimerManager().IsTimerActive(PanelRetryTimerHandle)) return;

    PC->GetWorldTimerManager().SetTimer(
        PanelRetryTimerHandle, this, &UComplyTeamStatusPanelsWidget::RetryPendingPanels, 0.5f, true
    );
}

void UComplyTeamStatusPanelsWidget::RetryPendingPanels()
{
    const APlayerController* LocalPC = GetOwningPlayer();
    if (!LocalPC || !LocalPC->IsLocalController()) return;

    AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
    if (!GS) return;

    for (APlayerState* PS : GS->PlayerArray)
    {
        if (PS == LocalPC->PlayerState) continue;

        AComplyPlayerState* ComplyPS = Cast<AComplyPlayerState>(PS);
        if (!ComplyPS) continue;

        // Skip if there's already a panel for this player
        bool bAlreadyAdded = false;
        for (UComplyTeammatePanelWidget* Panel : TeammateWidgets)
        {
            if (Panel && Panel->GetPlayerState() == ComplyPS)
            {
                bAlreadyAdded = true;
                break;
            }
        }
        if (bAlreadyAdded) continue;

        AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(PS->GetPawn());
        UAbilitySystemComponent* ASC = ComplyPS->GetAbilitySystemComponent();
        if (!Character || !ASC) continue;

        AddPanelForPlayer(ComplyPS, Character, ASC);
    }
}

void UComplyTeamStatusPanelsWidget::AddPanelForPlayer(AComplyPlayerState* ComplyPS, AComplyPlayerCharacter* Character, UAbilitySystemComponent* ASC)
{
    UComplyTeammatePanelWidget* Panel = CreateWidget<UComplyTeammatePanelWidget>(GetOwningPlayer(), TeammatePanelWidgetClass);
    if (!Panel) return;

    Panel->InitializePanel(ASC, Character, ComplyPS);

    if (UVerticalBoxSlot* BoxSlot = PanelsVerticalBox->AddChildToVerticalBox(Panel))
    {
        BoxSlot->SetPadding(FMargin(0.f));
    }

    TeammateWidgets.Add(Panel);
}

// When the vote kick resolves, if the target was kicked, remove its panel
void UComplyTeamStatusPanelsWidget::OnVoteKickResolved(bool bKicked, APlayerState* Target)
{
    if (!bKicked || !Target) return;

    for (UComplyTeammatePanelWidget* Panel : TeammateWidgets)
    {
        if (Panel && Panel->GetPlayerState() == Cast<AComplyPlayerState>(Target))
        {
            Panel->RemoveFromParent();
            TeammateWidgets.Remove(Panel);
            break;
        }
    }
}

void UComplyTeamStatusPanelsWidget::NativeDestruct()
{
    AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
    if (GS)
    {
        GS->OnVoteKickResolved.RemoveDynamic(this, &UComplyTeamStatusPanelsWidget::OnVoteKickResolved);
    }

    Super::NativeDestruct();
}
