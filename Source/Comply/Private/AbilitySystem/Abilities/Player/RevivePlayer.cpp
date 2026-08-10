// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/RevivePlayer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Data/Player/ComplyPlayerData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void URevivePlayer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Reviver = Cast<AComplyPlayerCharacter>(ActorInfo->AvatarActor.Get());
	checkf(Reviver->PlayerData, TEXT("PlayerData not set on %s"), *GetName());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AComplyPlayerCharacter* Avatar = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Avatar) { EndAbility(Handle, ActorInfo, ActivationInfo, true, false); return; }
	
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, TraceLength, TraceStart, TraceEnd, TraceDirection))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	
	FHitResult Hit;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(ReviveSphereTraceRadius);
	GetWorld()->SweepSingleByChannel(Hit, TraceStart, TraceEnd, FQuat::Identity, ECC_Interact, Sphere, QueryParams);
	
	if (AComplyPlayerCharacter* HitPlayer = Cast<AComplyPlayerCharacter>(Hit.GetActor()))
	{
		TargetPlayer = HitPlayer;

		if (Reviver && TargetPlayer)
		{
			// Rotates the reviver towards the player they are reviving on the server
			Reviver->Server_FaceTarget(TargetPlayer);
			
			// Locally rotates towards the player they are reviving
			FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Reviver->GetActorLocation(), TargetPlayer->GetActorLocation());
			Reviver->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));
			
			if (APlayerController* PC = Cast<APlayerController>(Avatar->GetController()))
			{
				PC->SetIgnoreMoveInput(true);
			}
		}
	}
	
	if (!Reviver || !TargetPlayer)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, NAME_None, Reviver->PlayerData->ReviveMontage);
	MontageTask->OnCompleted.AddDynamic(this, &URevivePlayer::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &URevivePlayer::OnMontageCancelled);
	MontageTask->OnCancelled.AddDynamic(this, &URevivePlayer::OnMontageCancelled);
	MontageTask->ReadyForActivation();
	
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_Reviving);
	
	FGameplayCueParameters CueParams;
	CueParams.Location = TargetPlayer->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::PlayerReviveStart, CueParams);
}

void URevivePlayer::OnMontageCompleted()
{
	if (Reviver && TargetPlayer->bIsDowned)
	{
		Reviver->Server_ReviveTarget(TargetPlayer);
		
		FGameplayCueParameters CueParams;
		CueParams.Location = TargetPlayer->GetActorLocation();
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::PlayerReviveEnd, CueParams);
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void URevivePlayer::OnMontageCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void URevivePlayer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (const AComplyPlayerCharacter* Avatar = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Avatar->GetController()))
		{
			PC->ResetIgnoreMoveInput();

			if (Reviver)
			{
				Reviver->SetActorRotation(FRotator(0.f, PC->GetControlRotation().Yaw, 0.f));
			}
		}
	}
	
	GetAbilitySystemComponentFromActorInfo()->SetTagMapCount(ComplyTags::States::State_Reviving, 0);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
