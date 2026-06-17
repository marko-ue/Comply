// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"

#include <filesystem>

#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/ComplyCharacterBase.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Framework/GameMode/ComplyGameModeBase.h"

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
		
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_Enemy);
		ObjectParams.AddObjectTypesToQuery(ECC_Shield);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		// Trace against the player too if friendly fire is enabled
		AComplyGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AComplyGameModeBase>();
		if (GameMode && GameMode->bFriendlyFire)
		{
			ObjectParams.AddObjectTypesToQuery(ECC_Player);
		}
		
		// A multi trace is used because overlap events are required, as well as direct hits for applying damage
		TArray<FHitResult> MultiHitResults;
		GetWorld()->LineTraceMultiByObjectType(MultiHitResults, Start, End, ObjectParams, CollisionParams);
		
		for (const FHitResult& Hit : MultiHitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (HitActor->ActorHasTag(FName("Shield")))
			{
				// An out parameter boolean is set to true if the overlapping actor is the dome shield
				OutPassedThroughShield = true;
				
				// Impact point and normal for the trace that overlapped with the shield actor
				FGameplayCueParameters CueParams;
				CueParams.Location = Hit.ImpactPoint;
				CueParams.Normal = Hit.ImpactNormal;
				UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
				GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::ShieldHitscanWeaponImpact, CueParams);
				
				continue;
			}

			// The trace hit result is also stored here
			TraceHitResult = Hit;
			
			if (CurrentActorInfo->IsLocallyControlled())
			{
				FGameplayCueParameters CueParams;
				CueParams.Location = Hit.ImpactPoint;
				CueParams.Normal = Hit.ImpactNormal;
				UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
					GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::HitscanWeaponImpact, CueParams);
			}
			
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
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_Enemy);
		ObjectParams.AddObjectTypesToQuery(ECC_Shield);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		// Trace against the player too if friendly fire is enabled
		AComplyGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AComplyGameModeBase>();
		if (GameMode && GameMode->bFriendlyFire)
		{
			ObjectParams.AddObjectTypesToQuery(ECC_Player);
		}
		
		// Target data for shotgun impact points
		FGameplayAbilityTargetDataHandle TargetDataHandle;
		
		TArray<FHitResult> ShieldHits;
		
		// A multi trace is used because overlap events are required, as well as direct hits for applying damage
		for (int32 i = 0; i < NumPellets; i++)
		{
			const FVector PelletDirection = FMath::VRandCone(CrosshairWorldDirection, FMath::DegreesToRadians(Ability->SpreadAngle));
			const FVector PelletEnd = Start + PelletDirection * TraceLength;

			TArray<FHitResult> MultiHitResults;
			GetWorld()->LineTraceMultiByObjectType(MultiHitResults, Start, PelletEnd, ObjectParams, CollisionParams);

			for (const FHitResult& Hit : MultiHitResults)
			{
				if (!Hit.GetActor()) continue;
				if (Hit.GetActor()->ActorHasTag(FName("Shield")))
				{
					// An out parameter boolean is set to true if the overlapping actor is the dome shield
					OutPassedThroughShield = true;
					ShieldHits.Add(Hit);
					continue;
				}
				OutHitResults.Add(Hit);
				
				// Hit result added to target data handle
				TargetDataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));
				break;
			}
		}
		
		// Execute cue outside the loop, pass in information from each hit
		if (ShieldHits.Num() > 0)
		{
			// Context with shotgun traces target data passed in, in order to only activate one cue for all shots
			FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
			FComplyGameplayEffectContext* Context = static_cast<FComplyGameplayEffectContext*>(ContextHandle.Get());
			if (Context)
			{
				// Pack shield hits into target data the same way it's done for regular pellets so only one cue is used
				FGameplayAbilityTargetDataHandle ShieldTargetData;
				for (const FHitResult& Hit : ShieldHits)
				{
					ShieldTargetData.Add(new FGameplayAbilityTargetData_SingleTargetHit(Hit));
				}
				Context->ShotgunTracesTargetData = ShieldTargetData;
			}

			FGameplayCueParameters CueParams;
			CueParams.EffectContext = ContextHandle;
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
				GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::ShieldShotgunImpact, CueParams);
		}
		
		/*
		 * One cue is created, and the context with the shotgun trace target data is created and passed in the params
		 * In the cue blueprint, we iterate over reach hit result to spawn particles at each location
		 * All the particles are spawned at once from one cue, so only one multicast RPC is needed
		 */
		FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FComplyGameplayEffectContext* Context = static_cast<FComplyGameplayEffectContext*>(ContextHandle.Get());
		if (Context)
		{
			Context->ShotgunTracesTargetData = TargetDataHandle;
		}
		
		if (CurrentActorInfo->IsLocallyControlled())
		{
			FGameplayCueParameters CueParams;
			CueParams.EffectContext = ContextHandle;
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(),ComplyTags::GameplayCues::ShotgunImpact, CueParams);
		}
		
		if (GetAbilitySystemComponentFromActorInfo()->IsOwnerActorAuthoritative() && !CurrentActorInfo->IsLocallyControlled())
		{
			FGameplayCueParameters CueParams;
			CueParams.EffectContext = ContextHandle;
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::ShotgunImpact, CueParams);
		}
	}
}

bool URangedWeaponAbilityBase::Fire()
{
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->bIsFiring)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return false;
	}
	
	// Find the active ranged weapon to get its current ammo
	if (Character)
	{
		ActiveWeapon = Character->GetEquippedPrimaryWeapon();
	}
	
	bool bFound = false;
	float CurrentAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentAmmoAttribute(), bFound);
	if (CurrentAmmo <= 0.f)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
		CueParams.Instigator = GetAvatarActorFromActorInfo();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::WeaponDryFire, CueParams);
		
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		
		if (GetAbilitySystemComponentFromActorInfo()->IsOwnerActorAuthoritative())
		{
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilityByClass(ReloadAbilityClass);
		}
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
	
	FGameplayCueParameters CueParams;
	CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
	CueParams.Instigator = GetAvatarActorFromActorInfo();
	
	// Execute the predicted cue non-replicated
	if (CurrentActorInfo->IsLocallyControlled())
	{
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
			GetAvatarActorFromActorInfo(),
			ComplyTags::GameplayCues::HitscanWeaponFire,
			CueParams
		);
	}

	// Only execute replicated cues on the server
	if (GetAbilitySystemComponentFromActorInfo()->IsOwnerActorAuthoritative() && !CurrentActorInfo->IsLocallyControlled())
	{
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
			ComplyTags::GameplayCues::HitscanWeaponFire, CueParams
		);
	}
	
	return true;
}

void URangedWeaponAbilityBase::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());

	for (const TSharedPtr<FGameplayAbilityTargetData>& Data : DataHandle.Data)
	{
		if (!Data.IsValid()) continue;

		if (Character->GetEquippedPrimaryWeapon()->bUsesSingleCrosshairTrace &&
			HasAuthority(&ActivationInfo))
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = Data->GetHitResult()->ImpactPoint;
			CueParams.Normal = Data->GetHitResult()->ImpactNormal;

			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
				ComplyTags::GameplayCues::HitscanWeaponImpact,
				CueParams);
		}

		AActor* TargetActor = Data->GetHitResult()->GetActor();

		if (TargetActor && HasAuthority(&ActivationInfo))
		{
			const FComplyGameplayAbilityTargetData_SingleHit* CustomData =
				static_cast<const FComplyGameplayAbilityTargetData_SingleHit*>(Data.Get());

			FComplyGameplayEffectContext* Context = new FComplyGameplayEffectContext();

			if (CustomData)
			{
				Context->bHitThroughShield = CustomData->bPassedThroughShield;
			}

			Context->ShieldDamageMultiplier = ShieldShotDamageMultiplier;

			float FinalDamage = Damage.GetValueAtLevel(GetAbilityLevel());

			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
			{
				int32 TotemStacks = ASC->GetTagCount(ComplyTags::States::State_TotemBuffed);
				FinalDamage *= (1.f + TotemDamageBonusPerStack * TotemStacks);
			}

			CauseDamage(TargetActor, FinalDamage, Context);
		}
	}

	if (!Character->GetEquippedPrimaryWeapon()->bUsesSingleCrosshairTrace &&
		HasAuthority(&ActivationInfo))
	{
		FGameplayEffectContextHandle ContextHandle =
			GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();

		FComplyGameplayEffectContext* Context =
			static_cast<FComplyGameplayEffectContext*>(ContextHandle.Get());

		if (Context)
		{
			Context->ShotgunTracesTargetData = DataHandle;
		}

		FGameplayCueParameters CueParams;
		CueParams.EffectContext = ContextHandle;

		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
			ComplyTags::GameplayCues::ShotgunImpact,
			CueParams);
	}
}

void URangedWeaponAbilityBase::OnFireDelayFinished()
{
	AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character || !Character->bIsFiring)
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
