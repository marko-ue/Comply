// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ComplyGameStateBase.generated.h"

/**
 * 
 */

USTRUCT()
struct FComplyVoteKickState
{
	GENERATED_BODY()

	UPROPERTY()
	APlayerState* TargetPlayer = nullptr;

	UPROPERTY()
	TArray<APlayerState*> VotersWhoVoted;

	UPROPERTY()
	int32 ApproveCount = 0;

	UPROPERTY()
	bool bVoteActive = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoteKickInitiated, APlayerState*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVoteKickResolved, bool, bKicked, APlayerState*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoteCountUpdated, int32, ApproveCount);


UCLASS()
class COMPLY_API AComplyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// This replicated variable is used for clients to check if friendly fire is enabled.
	UPROPERTY(Replicated)
	bool bFriendlyFire;
	
	// Used to globally track if all players have unique classes. Currently used so widgets can update their text without having to be constructed first
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Game State")
	bool bAllPlayersHaveUniqueClasses = false;
	
	// Vote kicking
	UFUNCTION()
	void InitiateVoteKick(APlayerState* Initiator, APlayerState* Target);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowVoteKickWidget(APlayerState* Target);

	UFUNCTION()
	void HandleVote(APlayerState* Voter, bool bApprove);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ResolveVote(bool bKicked, APlayerState* Target);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateVoteCount(int32 ApproveCount);
	
	UPROPERTY()
	FOnVoteKickInitiated OnVoteKickInitiated;

	UPROPERTY()
	FOnVoteKickResolved OnVoteKickResolved;
	
	UPROPERTY()
	FOnVoteCountUpdated OnVoteCountUpdated;

private:
	FComplyVoteKickState ActiveVote;
	
	FTimerHandle VoteKickTimerHandle;

	void OnVoteKickTimerExpired();
};
