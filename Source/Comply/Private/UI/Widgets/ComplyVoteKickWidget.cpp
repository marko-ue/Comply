// Copyright © 2026 Marko. All rights reserved.


#include "UI/Widgets/ComplyVoteKickWidget.h"

#include "ComplyPlayerController.h"
#include "Components/TextBlock.h"
#include "Framework/GameState/ComplyGameStateBase.h"
#include "GameFramework/PlayerState.h"


// When the vote initiates, the widget appears and the delegate is bound
void UComplyVoteKickWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);
	SetFocus();

	if (InstructionText)
	{
		InstructionText->SetText(FText::FromString(TEXT("F5 to approve | F10 to deny")));
	}

	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS)
	{
		GS->OnVoteKickResolved.AddDynamic(this, &UComplyVoteKickWidget::OnVoteResolved);
		GS->OnVoteCountUpdated.AddDynamic(this, &UComplyVoteKickWidget::OnVoteCountUpdated);
	}
	
	FWidgetAnimationDynamicEvent SlideOutAnimEvent;
	SlideOutAnimEvent.BindDynamic(this, &UComplyVoteKickWidget::OnSlideOutAnimFinished);
	BindToAnimationFinished(SlideOutAnim, SlideOutAnimEvent);
	
	if (VoteCountText)
	{
		VoteCountText->SetText(FText::FromString(TEXT("1 / 2 votes")));
	}
}

void UComplyVoteKickWidget::InitVote(APlayerState* Target)
{
	TargetPlayerState = Target;

	if (VotePromptText && Target)
	{
		const FString Prompt = FString::Printf(TEXT("Vote to kick %s?"), *Target->GetPlayerName());
		VotePromptText->SetText(FText::FromString(Prompt));
	}
	
	// Start countdown and update the text whenever the remaining time changes
	RemainingSeconds = 30;
	if (CountdownText)
	{
		CountdownText->SetText(FText::FromString(FString::Printf(TEXT("%ds left"), RemainingSeconds)));
	}

	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this,
		&UComplyVoteKickWidget::TickCountdown, 1.f, true
	);
	
	PlayAnimation(SlideInAnim);
}

void UComplyVoteKickWidget::TickCountdown()
{
	RemainingSeconds--;

	if (CountdownText)
	{
		CountdownText->SetText(FText::FromString(FString::Printf(TEXT("%ds left"), RemainingSeconds)));
	}

	if (RemainingSeconds <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}
}

void UComplyVoteKickWidget::OnVoteCountUpdated(int32 ApproveCount)
{
	if (VoteCountText)
	{
		const FString CountString = FString::Printf(TEXT("%d / 2 votes"), ApproveCount);
		VoteCountText->SetText(FText::FromString(CountString));
	}
}

void UComplyVoteKickWidget::OnVoteResolved(bool bKicked, APlayerState* Target)
{
	bKicked ? VoteCountText->SetColorAndOpacity(FSlateColor(FLinearColor(0.f, 1.f, 0.f, 1.f))) 
		: VoteCountText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f)));
	
	PlayAnimation(SlideOutAnim);
}

void UComplyVoteKickWidget::OnSlideOutAnimFinished()
{
	RemoveFromParent();
}

void UComplyVoteKickWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	
	AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS)
	{
		GS->OnVoteKickResolved.RemoveDynamic(this, &UComplyVoteKickWidget::OnVoteResolved);
		GS->OnVoteCountUpdated.RemoveDynamic(this, &UComplyVoteKickWidget::OnVoteCountUpdated);
	}

	Super::NativeDestruct();
}
