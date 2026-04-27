// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Character/ComplyPlayerCharacter.h"

class UComplyAttributeSet;


// Traces to the middle of the screen
void URangedWeaponAbilityBase::TraceToCrosshair(FHitResult& TraceHitResult, float TraceLength, bool& OutPassedThroughShield)
{
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

bool URangedWeaponAbilityBase::Fire()
{
	if (!SpendAmmoAndReload()) return false;
	
	// Any previous running hit scan target data tasks must be ended so it's not triggered for each accumulated task
	if (HitscanTargetDataTask)
	{
		HitscanTargetDataTask->EndTask();
	}
    
	HitscanTargetDataTask = UHitscanTargetData::CreateHitScanData(this);
	HitscanTargetDataTask->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	HitscanTargetDataTask->ReadyForActivation();
	
	return true;
}

void URangedWeaponAbilityBase::PlayAnimationBasedOnState()
{
	FGameplayTagContainer Tags;
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);
			
	if (Tags.HasTagExact(ComplyTags::States::State_Aiming))
	{
		PlayMontageAndBindDelegates(AbilityActivationMontageIronsights);
	}
	else
	{
		PlayMontageAndBindDelegates(AbilityActivationMontageHip);
	}
}

void URangedWeaponAbilityBase::HandleReload()
{
	if (PlayActivationMontageTask)
	{
		PlayActivationMontageTask->EndTask();
		PlayActivationMontageTask = nullptr;
	}
	if (ReloadMontageTask)
	{
		ReloadMontageTask->EndTask();
	}
		
	// Play reload animation
	ReloadMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ReloadMontage, 1.f, NAME_None, true);
		
	ReloadMontageTask->OnCompleted.AddDynamic(this, &URangedWeaponAbilityBase::OnReloadMontageCompleted);
	ReloadMontageTask->OnInterrupted.AddDynamic(this, &URangedWeaponAbilityBase::OnReloadMontageCompleted);
	ReloadMontageTask->OnCancelled.AddDynamic(this, &URangedWeaponAbilityBase::OnReloadMontageCompleted);
	ReloadMontageTask->OnBlendOut.AddDynamic(this, &URangedWeaponAbilityBase::OnReloadMontageCompleted);
		
	ReloadMontageTask->ReadyForActivation();
		
	// Effect that applies the reloading tag
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReloadStateEffectClass, 1.f, ContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

bool URangedWeaponAbilityBase::SpendAmmoAndReload()
{
	const UWeaponAttributeSet* WeaponAS = GetAbilitySystemComponentFromActorInfo()->GetSet<UWeaponAttributeSet>();
    
	if (WeaponAS->GetRifleCurrentAmmo() <= 0.f)
	{
		HandleReload();
		
		return false;
	}
	
	// Reduce ammo in mag by 1
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReduceAmmoEffectClass, 1.f, ContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	return true;
}

void URangedWeaponAbilityBase::OnReloadMontageCompleted()
{
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReloadEffectClass, 1.f, ContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	// Remove reloading tag
	FGameplayTagContainer Tags;
	Tags.AddTag(ComplyTags::States::State_Reloading);
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);
	
	// Resume firing if still holding, otherwise end the ability to allow future inputs (as it's instanced per actor)
	GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags(Tags);
	if (Tags.HasTagExact(ComplyTags::States::State_Firing))
	{
		Fire();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void URangedWeaponAbilityBase::PlayMontageAndBindDelegates(const TObjectPtr<UAnimMontage>& AnimationToPlay)
{
	checkf(AnimationToPlay, TEXT("Ability Activation Montage not set"));
	
	PlayActivationMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, NAME_None, AnimationToPlay, 1.f, NAME_None, true);
	
	PlayActivationMontageTask->OnCompleted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayActivationMontageTask->OnBlendOut.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayActivationMontageTask->OnCancelled.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	PlayActivationMontageTask->OnInterrupted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	
	PlayActivationMontageTask->ReadyForActivation();
}
