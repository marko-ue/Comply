// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Enforcer/Utility_Enforcer.h"
#include "AbilitySystemComponent.h"
#include "CableComponent.h"
#include "Comply.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "Character/Player/EnforcerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


void UUtility_Enforcer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Target data must be used so the client is pulled to the correct position
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->AbilityTargetDataSetDelegate(Handle, ActivationInfo.GetActivationPredictionKey())
		.AddUObject(this, &UUtility_Enforcer::OnTargetDataReceived);

	if (ActorInfo->IsLocallyControlled())
	{
		// Grapple trace is performed first to get the GrappleHit
		FHitResult GrappleHit;
		if (!PerformGrappleTrace(GrappleHit))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		// The grapple hit is passed into target data 
		FGameplayAbilityTargetDataHandle DataHandle;
		DataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(GrappleHit));
		
		ASC->ServerSetReplicatedTargetData(
			Handle,
			ActivationInfo.GetActivationPredictionKey(),
			DataHandle,
			FGameplayTag(),
			ASC->ScopedPredictionKey
		);

		OnTargetDataReceived(DataHandle, FGameplayTag());
	}
}

bool UUtility_Enforcer::PerformGrappleTrace(FHitResult& OutHitResult, const float GrappleRange) const
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return false;
    
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, GrappleRange, TraceStart, TraceEnd, TraceDirection)) return false;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Avatar);
   
	return GetWorld()->LineTraceSingleByChannel(OutHitResult, TraceStart, TraceEnd, ECC_GrappleTarget, CollisionParams);
}

// Now location related things can be used, as the server gets correct information from the client via target data
void UUtility_Enforcer::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ApplicationTag)
{
	GetAbilitySystemComponentFromActorInfo()->ConsumeClientReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActivationInfo().GetActivationPredictionKey()
	);
	
	if (!DataHandle.IsValid(0)) return;
	
	// Getting the hit result passed in by the client to check it against the tolerance
	const FHitResult* ClientHit = DataHandle.Get(0)->GetHitResult();
	if (!ClientHit)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
	
	const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();
	if (ActorInfo->IsNetAuthority() && !ActorInfo->IsLocallyControlled())
	{
		if (!CommitAbility(GetCurrentAbilitySpecHandle(), ActorInfo, GetCurrentActivationInfo()))
		{
			EndAbility(GetCurrentAbilitySpecHandle(), ActorInfo, GetCurrentActivationInfo(), true, true);
			return;
		}
	}

	const FHitResult* HitResult = DataHandle.Get(0)->GetHitResult();
	if (!HitResult)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); 
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false); 
		return;
	}

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	Player = Cast<AEnforcerCharacter>(GetAvatarActorFromActorInfo());
	if (Player)
	{
		Cable = Player->GrappleCable;
		if (Cable)
		{
			Player->GrappleAnchorPoint->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			Player->GrappleAnchorPoint->SetWorldLocation(HitResult->ImpactPoint);
			Cable->SetAttachEndToComponent(Player->GrappleAnchorPoint);
			Cable->bAttachEnd = true;
			Cable->SetVisibility(true);
		}
	}
	
	FGameplayCueParameters UseCueParams;
	UseCueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::GrapplingHookUse, UseCueParams);

	UAbilityTask_ApplyRootMotionMoveToForce* MoveTask =
		UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
			this, FName("GrapplePull"), HitResult->ImpactPoint, PullDuration,
			false, MOVE_Flying, false, PathOffsetCurve,
			ERootMotionFinishVelocityMode::ClampVelocity, FVector::ZeroVector, 500.f
		);

	MoveTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UUtility_Enforcer::OnPullReachedDestination);
	MoveTask->OnTimedOut.AddDynamic(this, &UUtility_Enforcer::OnPullTimedOut);
	MoveTask->ReadyForActivation();
	
	FGameplayCueParameters ImpactCueParams;
	ImpactCueParams.Location = HitResult->ImpactPoint;
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
		GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::GrapplingHookImpact, ImpactCueParams);
	
	// Gameplay cue starting the hooking looping sound started, removed when destination reached or hook pulled out
	FGameplayCueParameters HookingCueParams;
	HookingCueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo()->AddGameplayCue(
		ComplyTags::GameplayCues::GrapplingHookHooking, HookingCueParams);
}

void UUtility_Enforcer::OnPullReachedDestination()
{
	FinishGrapple();
}

// Duration expired without arriving (wall in the way, etc.). Still clean up normally
void UUtility_Enforcer::OnPullTimedOut()
{
	FinishGrapple();
}

void UUtility_Enforcer::FinishGrapple()
{
	if (ACharacter* Character = Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get()))
	{
		// Set movement mode back to falling so landing is handled naturally
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

// Ensuring cleanup whenever the ability ends
void UUtility_Enforcer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		if (CMC && CMC->MovementMode == MOVE_Flying)
		{
			CMC->SetMovementMode(MOVE_Falling);
		}
	}
	
	Player = Cast<AEnforcerCharacter>(GetAvatarActorFromActorInfo());
	if (Player)
	{
		Cable = Player->GrappleCable;
		if (Cable)
		{
			Cable->SetVisibility(false);
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(CableUpdateTimer);
	
	// The cue is removed here so both the server and client get it removed
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(ComplyTags::GameplayCues::GrapplingHookHooking);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
