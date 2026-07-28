// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Enforcer/Throwable_Enforcer.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "Actors/AbilityActors/DeployableTurret/DeployableTurretPreview.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Enforcer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ThrowOnServer(FVector::ZeroVector, FVector::ZeroVector);
}

void UThrowable_Enforcer::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	// The preview for the turret will be shown only on the owning client
	if (GetCurrentActorInfo()->IsLocallyControlled())
	{
		SpawnPreview(GetCurrentActorInfo());
	}
	
	if (GetCurrentActorInfo()->IsNetAuthority() && !GetCurrentActorInfo()->IsLocallyControlled())
	{
		// On the server, bind to the target data callback and wait for the client to send confirmed placement location via target data
		GetAbilitySystemComponentFromActorInfo()->AbilityTargetDataSetDelegate(
			GetCurrentAbilitySpecHandle(), GetCurrentActivationInfo().GetActivationPredictionKey()
		).AddUObject(this, &UThrowable_Enforcer::OnTargetDataReceived);
	}
}

void UThrowable_Enforcer::SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo)
{
	// Input is confirmed when the primary input is pressed again
	WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &UThrowable_Enforcer::PlayPlaceTurretAnimation);
	WaitConfirm->ReadyForActivation();
	
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	SpawnedTurretPreviewActor = GetWorld()->SpawnActor<ADeployableTurretPreview>(
		TurretPreviewActorClass,
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpawnedTurretPreviewActor)
	{
		SpawnedTurretPreviewActor->InitPreviewData(Cast<ACharacter>(ActorInfo->AvatarActor.Get()));
	}
}

void UThrowable_Enforcer::PlayPlaceTurretAnimation()
{
	UAbilityTask_PlayMontageAndWait* PlaceTurretMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlaceTurretMontage, 1.f, NAME_None, true);
	PlaceTurretMontageTask->OnCompleted.AddDynamic(this, &UThrowable_Enforcer::ConfirmThrow);
	PlaceTurretMontageTask->OnCancelled.AddDynamic(this, &UThrowable_Enforcer::PlaceTurretAnimationInterrupted);
	PlaceTurretMontageTask->OnInterrupted.AddDynamic(this, &UThrowable_Enforcer::PlaceTurretAnimationInterrupted);
	PlaceTurretMontageTask->ReadyForActivation();
	
    if (SpawnedTurretPreviewActor->bCanPlace)
    {
        SpawnedTurretPreviewActor->bShouldUpdatePosition = false;

        if (GetCurrentActorInfo()->IsLocallyControlled() && GetCurrentActorInfo()->IsNetAuthority())
        {
            // On the server the preview already exists locally. Replicate it for simulated proxies.
            SpawnedTurretPreviewActor->SetReplicates(true);
        }
        else if (GetCurrentActorInfo()->IsLocallyControlled() && !GetCurrentActorInfo()->IsNetAuthority())
        {
            // The client sends confirmed placement location and rotation to the server
            FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
            TargetData->HitResult = SpawnedTurretPreviewActor->LastPlacementHit;
        	TargetData->HitResult.ImpactNormal = SpawnedTurretPreviewActor->PlacementRotation.Vector();

            FGameplayAbilityTargetDataHandle TargetDataHandle;
            TargetDataHandle.Add(TargetData);

            if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
            {
                ASC->ServerSetReplicatedTargetData(
                    GetCurrentAbilitySpecHandle(), 
                    GetCurrentActivationInfo().GetActivationPredictionKey(),
                    TargetDataHandle,
                    FGameplayTag(),
                    ASC->ScopedPredictionKey
                );
            }
        }
        
        // Bypass ASC replication, trigger directly on local client only
        if (UGameplayCueManager* CueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager())
        {
            FGameplayCueParameters CueParams;
            CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
            CueManager->HandleGameplayCue(GetAvatarActorFromActorInfo(),
                ComplyTags::GameplayCues::TurretTyping,
                EGameplayCueEvent::WhileActive, CueParams);
        }
    }
    else
    {
        WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
        WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::PlayPlaceTurretAnimation);
        WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::PlayPlaceTurretAnimation);
        WaitConfirm->ReadyForActivation();
    }
}

void UThrowable_Enforcer::PlaceTurretAnimationInterrupted()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(ComplyTags::GameplayCues::TurretTyping);
	
	if (UGameplayCueManager* CueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		const FGameplayCueParameters CueParams;
		CueManager->HandleGameplayCue(GetAvatarActorFromActorInfo(),
			ComplyTags::GameplayCues::TurretTyping,
			EGameplayCueEvent::Removed, CueParams
		);
	}
}

// Handles spawning and replicating the preview actor at the client's sent placement location and rotation
void UThrowable_Enforcer::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ApplicationTag)
{
	GetAbilitySystemComponentFromActorInfo()->ConsumeClientReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActivationInfo().GetActivationPredictionKey()
	);

	if (!DataHandle.IsValid(0)) return;

	const FHitResult* HitResult = DataHandle.Get(0)->GetHitResult();
	if (!HitResult) return;
	
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	SpawnedTurretPreviewActor = GetWorld()->SpawnActor<ADeployableTurretPreview>(
		TurretPreviewActorClass, HitResult->ImpactPoint, HitResult->ImpactNormal.Rotation(), SpawnParams
	);

	if (SpawnedTurretPreviewActor)
	{
		SpawnedTurretPreviewActor->bShouldUpdatePosition = false;
		SpawnedTurretPreviewActor->SetReplicates(true);
	}
	
	UAbilityTask_PlayMontageAndWait* PlaceTurretMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlaceTurretMontage, 1.f, NAME_None, true);
	PlaceTurretMontageTask->OnCompleted.AddDynamic(this, &UThrowable_Enforcer::ConfirmThrow);
	PlaceTurretMontageTask->OnCancelled.AddDynamic(this, &UThrowable_Enforcer::PlaceTurretAnimationInterrupted);
	PlaceTurretMontageTask->OnInterrupted.AddDynamic(this, &UThrowable_Enforcer::PlaceTurretAnimationInterrupted);
	PlaceTurretMontageTask->ReadyForActivation();
}

void UThrowable_Enforcer::ConfirmThrow()
{
	GetAbilitySystemComponentFromActorInfo()->RemoveGameplayCue(ComplyTags::GameplayCues::TurretTyping);
	
	if (UGameplayCueManager* CueManager = UAbilitySystemGlobals::Get().GetGameplayCueManager())
	{
		const FGameplayCueParameters CueParams;
		CueManager->HandleGameplayCue(GetAvatarActorFromActorInfo(),
			ComplyTags::GameplayCues::TurretTyping,
			EGameplayCueEvent::Removed, CueParams
		);
	}
	
	if (!SpawnedTurretPreviewActor || !SpawnedTurretPreviewActor->bCanPlace)
        {
            // Re-create the task so the player can try placing again
            WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
            WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmThrow);
            WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelThrow);
            WaitConfirm->ReadyForActivation();
            return;
        }

	PlaceTurret();

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UThrowable_Enforcer::PlaceTurret()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;
	
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, 0.f, TraceStart, TraceEnd, TraceDirection)) return;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		FVector SpawnLocation = TraceStart;
		SpawnLocation = Hit.ImpactPoint;
	}
	
	// The turret will spawn rotated towards the crosshair's world direction rotation
	FRotator SpawnRotation = TraceDirection.Rotation();
	SpawnRotation.Yaw += 0.f;
	SpawnRotation.Pitch = 0.f;
	SpawnRotation.Roll = 0.f;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	// A server RPC is used to handle spawning the turret
	if (UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		ASC->Server_PlaceTurret(GetCurrentAbilitySpecHandle(), SpawnedTurretPreviewActor->PlacementLocation, SpawnedTurretPreviewActor->PlacementRotation);
	}
	
	// Destroy the preview actor now, as the actual turret is now placed
	if (SpawnedTurretPreviewActor)
	{
		SpawnedTurretPreviewActor->Destroy();
		SpawnedTurretPreviewActor = nullptr;
	}
}

void UThrowable_Enforcer::CancelThrow()
{
	if (SpawnedTurretPreviewActor)
	{
		SpawnedTurretPreviewActor->Destroy();
		SpawnedTurretPreviewActor = nullptr;
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UThrowable_Enforcer::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if (SpawnedTurretPreviewActor) SpawnedTurretPreviewActor->Destroy();
}

void UThrowable_Enforcer::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SpawnedTurretPreviewActor)
	{
		SpawnedTurretPreviewActor->Destroy();
		SpawnedTurretPreviewActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
