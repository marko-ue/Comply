// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "Character/ComplyCharacterBase.h"
#include "AbilitySystem/ComplyTags.h"

class UComplyAttributeSet;

// Traces to the middle of the screen
void URangedWeaponAbilityBase::TraceToCrosshair(FHitResult& TraceHitResult, float TraceLength, bool& OutPassedThroughShield)
{
	const FGameplayAbilityActivationInfo ActivationInfo;
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Avatar = GetAvatarActorFromActorInfo();

	if (!Avatar || !Owner) return;
	
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
		Start = CrosshairWorldPosition;
		
		if (Avatar)
		{
			float DistanceToCharacter = (Avatar->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100);
		}
		
		End = Start + CrosshairWorldDirection * TraceLength;

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Avatar);
		
		// A multi trace is used because overlap events are required, as well as direct hits for applying damage
		TArray<FHitResult> MultiHitResults;
		GetWorld()->LineTraceMultiByChannel(
			MultiHitResults,
			Start,
			End,
			ECC_Pawn,
			CollisionParams
		);
		
		for (const FHitResult& Hit : MultiHitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (HitActor->ActorHasTag(FName("Shield")))
			{
				// An out parameter boolean is set to true if the overlapping actor is the dome shield
				OutPassedThroughShield = true;
				continue;
			}

			// The trace hit result is also stored here
			TraceHitResult = Hit;
			return;
		}
	}
}

void URangedWeaponAbilityBase::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	AActor* TargetActor = DataHandle.Data[0]->GetHitResult()->GetActor();
	
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (TargetActor && HasAuthority(&ActivationInfo))
	{
		// A multiplier parameter is used on the CauseDamage function
		// In this case, the multiplier is affected by whether the hit passed through a shield
		const float Multiplier = HitscanTargetDataTask->bPassedThroughShield ? ShieldShotDamageMultiplier : 1.f;
		CauseDamage(TargetActor, Multiplier);
	}
}

void URangedWeaponAbilityBase::OnFireDelayFinished()
{
	FGameplayTagContainer Tags;
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);
	if (!Tags.HasTagExact(ComplyTags::States::State_Firing))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	else
	{
		Fire();
	}
}

void URangedWeaponAbilityBase::Fire()
{
	// Creates the task, binds the delegate which will trigger when data is received to the server, and activates it
	HitscanTargetDataTask = UHitscanTargetData::CreateHitScanData(this);
	HitscanTargetDataTask->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	HitscanTargetDataTask->ReadyForActivation();
}

void URangedWeaponAbilityBase::PlayAnimationBasedOnState()
{
	AComplyCharacterBase* Character = Cast<AComplyCharacterBase>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		if (Character)
		{
			FGameplayTagContainer Tags;
			Character->GetAbilitySystemComponent()->GetOwnedGameplayTags(Tags);
			
			if (Tags.HasTagExact(ComplyTags::States::State_Aiming))
			{
				PlayMontageAndBindDelegates(AbilityActivationMontageIronsights);
			}
			else
			{
				PlayMontageAndBindDelegates(AbilityActivationMontageHip);
			}
		}
	}
}

void URangedWeaponAbilityBase::PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay)
{
	checkf(AnimationToPlay, TEXT("Ability Activation Montage not set"));
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, NAME_None, AnimationToPlay, 1.f, NAME_None, true);
	PlayMontageTask->OnCompleted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayMontageTask->OnCancelled.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	PlayMontageTask->ReadyForActivation();
}


