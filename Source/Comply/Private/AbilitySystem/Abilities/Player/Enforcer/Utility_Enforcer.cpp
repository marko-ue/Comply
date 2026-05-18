// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Enforcer/Utility_Enforcer.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UUtility_Enforcer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	FHitResult GrappleHit;
	if (!PerformGrappleTrace(GrappleHit))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Set the character's movement mode to flying so gravity is disabled while pulling
	// This makes the trajectory straight to the target location
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	/* 
	 * The task simply moves this ability's owning actor to the target location smoothly
	 * It also allows for further customization
	 * All this is handled through the character movement component, so replication is handled through it
	*/
	UAbilityTask_ApplyRootMotionMoveToForce* MoveTask =
		UAbilityTask_ApplyRootMotionMoveToForce::ApplyRootMotionMoveToForce(
			this,
			FName("GrapplePull"),
			GrappleHit.ImpactPoint, // Will pull towards the impact point of the grapple trace (hitscan from camera)
			PullDuration, // The player will be pulled for at most the pull duration, before the pull is stopped
			false,
			MOVE_Flying,
			false,
			PathOffsetCurve, // Curve to give a swing instead of a straight pull
			ERootMotionFinishVelocityMode::ClampVelocity,
			FVector::ZeroVector,
			500.f
		);

	MoveTask->OnTimedOutAndDestinationReached.AddDynamic(this, &UUtility_Enforcer::OnPullReachedDestination);
	MoveTask->OnTimedOut.AddDynamic(this, &UUtility_Enforcer::OnPullTimedOut);
	MoveTask->ReadyForActivation();
}

// Ensuring cleanup whenever the ability ends. Also ensures cleanup happens no matter how the ability was ended
void UUtility_Enforcer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		UCharacterMovementComponent* CMC = Character->GetCharacterMovement();
		if (CMC && CMC->MovementMode == MOVE_Flying)
		{
			CMC->SetMovementMode(MOVE_Falling);
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UUtility_Enforcer::PerformGrappleTrace(FHitResult& OutHitResult, float GrappleRange)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Avatar = GetAvatarActorFromActorInfo();

	if (!Avatar || !Owner) return false;
    
	FVector2D ViewportSize = FVector2D();
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
    
	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(
	   this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
       
		if (Avatar)
		{
			float DistanceToCharacter = (Avatar->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100);
		}
       
		FVector End = Start + CrosshairWorldDirection * GrappleRange;

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Avatar);
       
		return GetWorld()->LineTraceSingleByChannel(
		   OutHitResult,
		   Start,
		   End,
		   ECC_WorldStatic,
		   CollisionParams
		);
	}

	return false;
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

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
			   GetCurrentActivationInfo(), true, false);
}
