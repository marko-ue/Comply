// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"
#include "Character/ComplyCharacterBase.h"
#include "Character/ComplyPlayerCharacter.h"

// Traces to the middle of the screen
// This function is called in HitscanTargetData for transferring hitscan data from client to server
void URangedWeaponAbilityBase::TraceToCrosshair(FHitResult& TraceHitResult, const float TraceLength, bool& OutPassedThroughShield)
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
		GetWorld()->LineTraceMultiByChannel(MultiHitResults, Start, End, ECC_Pawn, CollisionParams);
		
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

void URangedWeaponAbilityBase::PerformShotgunTraces(TArray<FHitResult>& OutHitResults, const int32 NumPellets, const float TraceLength, bool& OutPassedThroughShield)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Avatar = GetAvatarActorFromActorInfo();

	const UPrimary_Disruptor* Ability = Cast<UPrimary_Disruptor>(GetCurrentAbilitySpec()->GetPrimaryInstance());

	if (!Avatar || !Owner || !Ability) return;
	
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
		for (int32 i = 0; i < NumPellets; i++)
		{
			const FVector PelletDirection = FMath::VRandCone(CrosshairWorldDirection, FMath::DegreesToRadians(Ability->SpreadAngle));
			const FVector PelletEnd = Start + PelletDirection * TraceLength;

			TArray<FHitResult> MultiHitResults;
			GetWorld()->LineTraceMultiByChannel(MultiHitResults, Start, PelletEnd, ECC_Pawn, CollisionParams);

			for (const FHitResult& Hit : MultiHitResults)
			{
				if (!Hit.GetActor()) continue;
				if (Hit.GetActor()->ActorHasTag(FName("Shield")))
				{
					OutPassedThroughShield = true;
					continue;
				}
				OutHitResults.Add(Hit);
				break; // Stop at first solid hit per pellet
			}
		}
	}
}

bool URangedWeaponAbilityBase::Fire()
{
	// Find the active ranged weapon to get its current ammo
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character)
	{
		ActiveWeapon = Character->GetEquippedPrimaryWeapon();
		Character->bIsFiring = true;
	}
	
	bool bFound = false;
	float CurrentAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentAmmoAttribute(), bFound);
	if (CurrentAmmo <= 0.f)
	{
		GetAbilitySystemComponentFromActorInfo()->TryActivateAbilityByClass(ReloadAbilityClass);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	
	// Reduce ammo in mag by 1
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReduceAmmoEffectClass, 1.f, ContextHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	// Any previous running hit scan target data tasks must be ended so it's not triggered for each accumulated task
	if (HitscanTargetDataTask)
	{
		HitscanTargetDataTask->EndTask();
	}
    
	HitscanTargetDataTask = UHitscanTargetData::CreateHitScanData(this);
	HitscanTargetDataTask->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	HitscanTargetDataTask->ReadyForActivation();
	
	if (RangedWeaponType == ERangedWeaponType::Automatic)
	{
		if (FireDelayTask)
		{
			FireDelayTask->EndTask();
		}

		FireDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, FireInterval);
		FireDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnFireDelayFinished);
		FireDelayTask->ReadyForActivation();
	}
	
	return true;
}

void URangedWeaponAbilityBase::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

	for (const TSharedPtr<FGameplayAbilityTargetData>& Data : DataHandle.Data)
	{
		if (!Data.IsValid()) continue;

		AActor* TargetActor = Data->GetHitResult()->GetActor();
		if (TargetActor && HasAuthority(&ActivationInfo))
		{
			FComplyGameplayEffectContext* Context = new FComplyGameplayEffectContext();
			Context->bHitThroughShield = HitscanTargetDataTask->bPassedThroughShield;
			Context->ShieldDamageMultiplier = ShieldShotDamageMultiplier;

			float FinalDamage = Damage.GetValueAtLevel(GetAbilityLevel());

			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
			{
				// Increases damage based on the amount of stacks of totem buffs
				int32 TotemStacks = ASC->GetTagCount(ComplyTags::States::State_TotemBuffed);
				FinalDamage *= (1.f + TotemDamageBonusPerStack * TotemStacks);
			}

			CauseDamage(TargetActor, FinalDamage, Context);
		}
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
	if (PlayActivationMontageTask)
	{
		PlayActivationMontageTask->EndTask();
	}
	
	checkf(AnimationToPlay, TEXT("Ability Activation Montage not set"));
	
	PlayActivationMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
	this, NAME_None, AnimationToPlay, 1.f, NAME_None, true);
	
	PlayActivationMontageTask->OnCompleted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayActivationMontageTask->OnBlendOut.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayActivationMontageTask->OnCancelled.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	PlayActivationMontageTask->OnInterrupted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	
	PlayActivationMontageTask->ReadyForActivation();
}

void URangedWeaponAbilityBase::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	GetAbilitySystemComponentFromActorInfo()->RemoveReplicatedLooseGameplayTag(ComplyTags::States::State_Firing);
}

void URangedWeaponAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
										  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
										  bool bReplicateEndAbility, bool bWasCancelled)
{
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Character) Character->bIsFiring = false;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
