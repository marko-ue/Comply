// Copyright © 2026 Marko. All rights reserved.


#include "Framework/GameState/ComplyGameStateBase.h"

#include "ComplyPlayerController.h"
#include "Framework/GameInstance/ComplyGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


void AComplyGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		const UComplyGameInstance* GI = GetGameInstance<UComplyGameInstance>();

		if (GI)
		{
			bFriendlyFire = GI->bFriendlyFire;
		}
	}
}

void AComplyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, bFriendlyFire);
	DOREPLIFETIME(ThisClass, bAllPlayersHaveUniqueClasses);
}

// Function called when the vote kick is initiated by a player. All struct variables are filled now
void AComplyGameStateBase::InitiateVoteKick(APlayerState* Initiator, APlayerState* Target)
{
	if (ActiveVote.bVoteActive || !Target) return;

	ActiveVote.TargetPlayer = Target;
	ActiveVote.ApproveCount = 1; // The initiator's vote is approved automatically
	ActiveVote.VotersWhoVoted.Empty();
	ActiveVote.VotersWhoVoted.Add(Initiator); // Immediately add the initiator to the list of who voted
	ActiveVote.bVoteActive = true;
	
	GetWorldTimerManager().SetTimer(VoteKickTimerHandle, this, &AComplyGameStateBase::OnVoteKickTimerExpired, 30.f, false);

	Multicast_ShowVoteKickWidget(Target);
}

void AComplyGameStateBase::Multicast_ShowVoteKickWidget_Implementation(APlayerState* Target)
{
	// Each client will bind to this and show their widget
	OnVoteKickInitiated.Broadcast(Target);
}

void AComplyGameStateBase::HandleVote(APlayerState* Voter, bool bApprove)
{
	if (!ActiveVote.bVoteActive || !Voter) return;
	if (ActiveVote.VotersWhoVoted.Contains(Voter)) return; // No double voting
	if (Voter == ActiveVote.TargetPlayer) return; // Target can't vote

	ActiveVote.VotersWhoVoted.Add(Voter);
	if (bApprove) ActiveVote.ApproveCount++;

	// If 2 players vote to kick (excluding the target), remove that player, broadcast the resolve and reset the struct
	if (ActiveVote.ApproveCount >= 2)
	{
		AComplyPlayerController* PC = Cast<AComplyPlayerController>(ActiveVote.TargetPlayer->GetOwner());
		if (PC) PC->ClientTravel(TEXT("/Game/Maps/Lobby"), TRAVEL_Absolute);

		Multicast_ResolveVote(true, ActiveVote.TargetPlayer);
		ActiveVote = FComplyVoteKickState(); // Reset struct
	}
}

// If the vote kick timer expires, that means the vote failed. Pass in false to the multicast.
void AComplyGameStateBase::OnVoteKickTimerExpired()
{
	if (!ActiveVote.bVoteActive) return;
	
	Multicast_ResolveVote(false, ActiveVote.TargetPlayer);
	ActiveVote = FComplyVoteKickState();
}

void AComplyGameStateBase::Multicast_UpdateVoteCount_Implementation(int32 ApproveCount)
{
	OnVoteCountUpdated.Broadcast(ApproveCount);
}

void AComplyGameStateBase::Multicast_ResolveVote_Implementation(bool bKicked, APlayerState* Target)
{
	OnVoteKickResolved.Broadcast(bKicked, Target);
}
