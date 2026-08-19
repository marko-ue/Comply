// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ComplyVoteKickWidget.generated.h"

class UTextBlock;
class APlayerState;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyVoteKickWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitVote(APlayerState* Target);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnVoteResolved(bool bKicked, APlayerState* Target);
	
	UFUNCTION()
	void OnVoteCountUpdated(int32 ApproveCount);
	
	UFUNCTION()
	void OnSlideOutAnimFinished();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VotePromptText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InstructionText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> VoteCountText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountdownText;
	
	FTimerHandle CountdownTimerHandle;
	int32 RemainingSeconds = 0;
	
	void TickCountdown();

	UPROPERTY()
	TObjectPtr<APlayerState> TargetPlayerState;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SlideOutAnim;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> SlideInAnim;
};
