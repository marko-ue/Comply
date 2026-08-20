// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "Comply.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/Player/Disruptor/Primary_Disruptor.h"
#include "AbilitySystem/Data/Player/Damage/ComplyDamageData.h"
#include "AbilitySystem/Data/Player/Weapons/ShotgunWeaponData.h"
#include "Character/ComplyCharacterBase.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Framework/GameMode/ComplyGameModeBase.h"
#include "Framework/GameState/ComplyGameStateBase.h"


// Traces to the middle of the screen
// This function is called in HitscanTargetData for transferring hitscan data from client to server
void URangedWeaponAbilityBase::TraceToCrosshair(FHitResult& TraceHitResult, const float TraceLength, bool& OutPassedThroughShield)
{
	const AActor* Owner = GetOwningActorFromActorInfo();
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar || !Owner) return;
	
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, TraceLength, TraceStart, TraceEnd, TraceDirection)) return;
	
	FCollisionQueryParams CollisionParams;
	FCollisionObjectQueryParams ObjectParams;
	BuildWeaponCollisionParams(Avatar, CollisionParams, ObjectParams);

	TArray<FHitResult> MultiHitResults;
	GetWorld()->LineTraceMultiByObjectType(MultiHitResults, TraceStart, TraceEnd, ObjectParams, CollisionParams);
	
	for (const FHitResult& Hit : MultiHitResults)
	{
		const AActor* HitActor = Hit.GetActor();
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
				GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::ShieldHitscanWeaponImpact, CueParams
			);
			
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
				GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::HitscanWeaponImpact, CueParams
			);
		}
		
		return;
	}
}

void URangedWeaponAbilityBase::PerformShotgunTraces(TArray<FHitResult>& OutHitResults, const int32 NumPellets, const float TraceLength, bool& OutPassedThroughShield)
{
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Avatar = GetAvatarActorFromActorInfo();
	const UPrimary_Disruptor* Ability = Cast<UPrimary_Disruptor>(GetCurrentAbilitySpec()->GetPrimaryInstance());
	if (!Avatar || !Owner || !Ability) return;
	
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, TraceLength, TraceStart, TraceEnd, TraceDirection)) return;
	
	FCollisionQueryParams CollisionParams;
	FCollisionObjectQueryParams ObjectParams;
	BuildWeaponCollisionParams(Avatar, CollisionParams, ObjectParams);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TArray<FHitResult> ShieldHits;
	// A multi trace is used because overlap events are required, as well as direct hits for applying damage
	for (int32 i = 0; i < NumPellets; i++)
	{
		const FVector PelletDirection = FMath::VRandCone(TraceDirection, FMath::DegreesToRadians(Ability->ShotgunWeaponData->SpreadAngle));
		const FVector PelletEnd = TraceStart + PelletDirection * TraceLength;

		TArray<FHitResult> MultiHitResults;
		GetWorld()->LineTraceMultiByObjectType(MultiHitResults, TraceStart, PelletEnd, ObjectParams, CollisionParams);
		
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
			GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::ShieldShotgunImpact, CueParams
		);
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
		FScopedPredictionWindow ScopedPrediction(GetAbilitySystemComponentFromActorInfo(),
			CurrentActivationInfo.GetActivationPredictionKey()
		);
		
		FGameplayCueParameters CueParams;
		CueParams.EffectContext = ContextHandle;
		GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::ShotgunImpact, CueParams);
	}
}

// Helper function that sets appropriate query and object params
void URangedWeaponAbilityBase::BuildWeaponCollisionParams(const AActor* Avatar, FCollisionQueryParams& OutQueryParams, FCollisionObjectQueryParams& OutObjectParams)
{
	OutQueryParams.AddIgnoredActor(Avatar);

	OutObjectParams.AddObjectTypesToQuery(ECC_Enemy);
	OutObjectParams.AddObjectTypesToQuery(ECC_Shield);
	OutObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);

	// Trace against the player too if friendly fire is enabled
	const AComplyGameStateBase* GS = GetWorld()->GetGameState<AComplyGameStateBase>();
	if (GS && GS->bFriendlyFire)
	{
		OutObjectParams.AddObjectTypesToQuery(ECC_Player);
		OutObjectParams.AddObjectTypesToQuery(ECC_PlayerFriend);
	}
}

bool URangedWeaponAbilityBase::Fire()
{
	checkf(WeaponData, TEXT("WeaponData not set on %s"), *GetName());
	checkf(WeaponData->DamageData, TEXT("DamageData not set on %s"), *GetName());

	// Ensure all reload abilities are canceled when firing, so cleanup code always runs
	// Mostly helps with removing the Reloading tag when firing is interrupted while reloading the shotgun
	FGameplayTagContainer ReloadTag;
	ReloadTag.AddTag(ComplyTags::ComplyAbilities::Reload);
	GetAbilitySystemComponentFromActorInfo()->CancelAbilities(&ReloadTag);
	
	if (AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		ActiveWeapon = Character->GetEquippedPrimaryWeapon();
		
		// Automatic weapons directly use this variable for checking whether firing should continue
		// so it's set to true when starting fire for them. For non-automatic weapons, this variable is only used to handle player rotation
		if (WeaponData->RangedWeaponType == ERangedWeaponType::Automatic)
		{
			Character->bIsFiring = true;
		}
	}

	if (CommitAbilityCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		if (AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo()))
		{
			Character->ApplyFiringFeedback(WeaponData);

			UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
			const float CurrentAmmo = ASC->GetNumericAttribute(ActiveWeapon->GetCurrentAmmoAttribute());
			const float ReserveAmmo = ASC->GetNumericAttribute(ActiveWeapon->GetCurrentReserveAmmoAttribute());
			if (CurrentAmmo <= 0.f)
			{
				ASC->AddLooseGameplayTag(ComplyTags::States::State_FiringBlocked);
				
				// Only attempt reload if there's reserve ammo available
				if (ReserveAmmo > 0.f)
				{
					FTimerHandle ReloadTimer;
					GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]()
					{
						GetAbilitySystemComponentFromActorInfo()->TryActivateAbilityByClass(WeaponData->ReloadAbilityClass);
					}, 0.1f, false);
				}
				else
				{
					// No reserve ammo, play dry fire and end the ability to prevent firing when auto reload is not possible
					FGameplayCueParameters CueParams;
					CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
					CueParams.Instigator = GetAvatarActorFromActorInfo();
					UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::WeaponDryFire, CueParams);
        
					EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
				}
			}
		}
	}
	
	// Any previous running hit scan target data tasks must be ended so it's not triggered for each accumulated task
	if (HitscanTargetDataTask)
	{
		HitscanTargetDataTask->EndTask();
	}
    
	HitscanTargetDataTask = UHitscanTargetData::CreateHitScanData(this);
	HitscanTargetDataTask->ValidData.AddDynamic(this, &ThisClass::OnTargetDataReceived);
	HitscanTargetDataTask->ReadyForActivation();
	
	if (WeaponData->RangedWeaponType == ERangedWeaponType::Automatic)
	{
		if (FireDelayTask)
		{
			FireDelayTask->EndTask();
		}

		FireDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, WeaponData->FireInterval);
		FireDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnFireDelayFinished);
		FireDelayTask->ReadyForActivation();
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	CueParams.Instigator = GetAvatarActorFromActorInfo();
	
	// Execute the local cue non-replicated
	if (CurrentActorInfo->IsLocallyControlled())
	{
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated( 
			GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::HitscanWeaponFire, CueParams
		);
	}

	// Execute replicated cue; prediction key lets owning client skip re-execution
	if (GetAbilitySystemComponentFromActorInfo()->IsOwnerActorAuthoritative() && !IsLocallyControlled())
	{
		FScopedPredictionWindow ScopedPrediction(
			GetAbilitySystemComponentFromActorInfo(), CurrentActivationInfo.GetActivationPredictionKey()
		);
		
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
    
    const FVector MuzzleLocation = Character->WeaponMesh->GetSocketLocation(FName("MuzzleFlash"));

    // Calculate damage once so both single and shotgun blocks can use it
    float FinalDamage = WeaponData->DamageData->Damage.GetValueAtLevel(GetAbilityLevel());

    if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        const int32 TotemStacks = ASC->GetTagCount(ComplyTags::States::State_TotemBuffed);
        FinalDamage *= (1.f + WeaponData->DamageData->TotemDamageBonusPerStack * TotemStacks);
    }

    for (const TSharedPtr<FGameplayAbilityTargetData>& Data : DataHandle.Data)
    {
        if (!Data.IsValid()) continue;
        
        if (IsLocallyControlled())
        {
            Character->SpawnImpactEffectsLocal(Data->GetHitResult()->ImpactPoint, Data->GetHitResult()->ImpactNormal, MuzzleLocation, WeaponData);
        }

        if (Character->GetEquippedPrimaryWeapon()->WeaponData->bUsesSingleCrosshairTrace &&
            HasAuthority(&ActivationInfo))
        {
            FGameplayCueParameters CueParams;
            CueParams.Location = Data->GetHitResult()->ImpactPoint;
            CueParams.Normal = Data->GetHitResult()->ImpactNormal;

            GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
                ComplyTags::GameplayCues::HitscanWeaponImpact,
                CueParams
            );
            
            Character->Multicast_SpawnImpactEffects(Data->GetHitResult()->ImpactPoint, Data->GetHitResult()->ImpactNormal, MuzzleLocation, WeaponData);
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

            Context->ShieldDamageMultiplier = WeaponData->ShieldShotDamageMultiplier;

            CauseDamage(TargetActor, FinalDamage, Context);
        	
        	if (Cast<AComplyCharacterBase>(TargetActor))
        	{
        		// Checks if the hit passed through a shield, and if so, include that in the damage calculation to show as the damage number and pass in the appropriate color
        		const bool bShieldHit = CustomData && CustomData->bPassedThroughShield;
        		const float DisplayDamage = bShieldHit ? FinalDamage * WeaponData->ShieldShotDamageMultiplier : FinalDamage;
        		const FLinearColor DisplayColor = bShieldHit ? FLinearColor(0.f, 0.8f, 0.f, 1.f) : FLinearColor(1.f, 0.35f, 0.f, 1.f);
        		Character->Client_ShowDamageNumber(DisplayDamage, Data->GetHitResult()->ImpactPoint, DisplayColor);
        	}
        }
    }
    
    if (IsLocallyControlled())
    {
        for (const TSharedPtr<FGameplayAbilityTargetData>& Data : DataHandle.Data)
        {
            Character->SpawnImpactEffectsLocal(Data->GetHitResult()->ImpactPoint, Data->GetHitResult()->ImpactNormal, MuzzleLocation, WeaponData);
        }
    }

    if (!Character->GetEquippedPrimaryWeapon()->WeaponData->bUsesSingleCrosshairTrace && HasAuthority(&ActivationInfo))
    {
        FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();

        FComplyGameplayEffectContext* Context = static_cast<FComplyGameplayEffectContext*>(ContextHandle.Get());

        if (Context)
        {
            Context->ShotgunTracesTargetData = DataHandle;
        }

        FGameplayCueParameters CueParams;
        CueParams.EffectContext = ContextHandle;

        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(
            ComplyTags::GameplayCues::ShotgunImpact,
            CueParams
        );
        
        for (const TSharedPtr<FGameplayAbilityTargetData>& Data : DataHandle.Data)
        {
            AActor* TargetActor = Data->GetHitResult()->GetActor();
            
            if (!Data.IsValid() || !Data->GetHitResult()->bBlockingHit) continue;
        	
        	Character->Multicast_SpawnImpactEffects(Data->GetHitResult()->ImpactPoint, Data->GetHitResult()->ImpactNormal, MuzzleLocation, WeaponData);
        	
        	if (const AComplyCharacterBase* HitCharacter = Cast<AComplyCharacterBase>(TargetActor))
        	{
        		const FComplyGameplayAbilityTargetData_SingleHit* CustomData =
					static_cast<const FComplyGameplayAbilityTargetData_SingleHit*>(Data.Get());

        		// Checks if the hit passed through a shield, and if so, include that in the damage calculation to show as the damage number and pass in the appropriate color
        		const bool bShieldHit = CustomData && CustomData->bPassedThroughShield;
        		const float DisplayDamage = bShieldHit ? FinalDamage * WeaponData->ShieldShotDamageMultiplier : FinalDamage;
        		const FLinearColor DisplayColor = bShieldHit ? FLinearColor(0.f, 0.8f, 0.f, 1.f) : FLinearColor(1.f, 0.35f, 0.f, 1.f);

        		const FVector Offset = FVector(
					FMath::RandRange(-5.f, 5.f),
					FMath::RandRange(-5.f, 5.f),
					FMath::RandRange(60.f, 80.f)
				);
        		Character->Client_ShowDamageNumber(DisplayDamage, HitCharacter->GetActorLocation() + Offset, DisplayColor);
        	}
        }
    }
}

void URangedWeaponAbilityBase::OnFireDelayFinished()
{
	if (!FireDelayTask) return; // Already handled
    
	FireDelayTask = nullptr; // Clear before calling Fire to prevent re-entry

	const AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return;

	if (GetAbilitySystemComponentFromActorInfo()->IsOwnerActorAuthoritative())
	{
		// Server checks elapsed time against how long client actually held input
		const float ElapsedFireTime = GetWorld()->GetTimeSeconds() - FireStartTime;
		if (ElapsedFireTime >= Character->FireInputHeldDuration)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}
	}
	else
	{
		if (!Character->bIsFiring)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}
	}

	Fire();
}

void URangedWeaponAbilityBase::PlayAnimationBasedOnState()
{
	if (const AComplyCharacterBase* Character = Cast<AComplyCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		if (Character)
		{
			FGameplayTagContainer Tags;
			Character->GetAbilitySystemComponent()->GetOwnedGameplayTags(Tags);
			
			if (Tags.HasTagExact(ComplyTags::States::State_Aiming))
			{
				PlayMontageAndBindDelegates(WeaponData->AbilityActivationMontageIronsights);
			}
			else
			{
				PlayMontageAndBindDelegates(WeaponData->AbilityActivationMontageHip);
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

	PlayActivationMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AnimationToPlay, 1.f, NAME_None, true);

	PlayActivationMontageTask->OnCompleted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayActivationMontageTask->OnBlendOut.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCompleted);
	PlayActivationMontageTask->OnCancelled.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);
	PlayActivationMontageTask->OnInterrupted.AddDynamic(this, &URangedWeaponAbilityBase::OnMontageCancelled);

	PlayActivationMontageTask->ReadyForActivation();
	
	if (WeaponData->RangedWeaponType != ERangedWeaponType::Automatic)
    {
        GetAbilitySystemComponentFromActorInfo()->SetLooseGameplayTagCount(
            ComplyTags::States::State_FiringBlocked, 1
        );
    }
}

void URangedWeaponAbilityBase::OnMontageCompleted()
{
	// Remove tag and end ability so firing can be activated again
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_FiringBlocked);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	
	Super::OnMontageCompleted();
}

void URangedWeaponAbilityBase::OnMontageCancelled()
{
	// Remove tag and end ability so firing can be activated again
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_FiringBlocked);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	
	Super::OnMontageCancelled();
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
	if (AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->bIsFiring = false;
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
