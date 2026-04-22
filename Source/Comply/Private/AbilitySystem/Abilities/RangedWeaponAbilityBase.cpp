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
void URangedWeaponAbilityBase::TraceToCrosshair(FHitResult& TraceHitResult, float TraceLength)
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
		
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Pawn,
			CollisionParams
		);
	}
}

void URangedWeaponAbilityBase::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	// Takes the actor hit by the trace and causes damage (server)
	AActor* TargetActor = DataHandle.Data[0]->GetHitResult()->GetActor();
	if (TargetActor)
	{
		CauseDamage(TargetActor);
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


