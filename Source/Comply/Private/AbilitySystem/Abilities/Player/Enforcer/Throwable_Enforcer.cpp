// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Enforcer/Throwable_Enforcer.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/HitscanTargetData.h"
#include "Actors/DeployableTurret/DeployableTurretPreview.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Enforcer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Throw();
}

void UThrowable_Enforcer::Throw()
{
	// The preview for the turret will be shown only on the owning client
	if (GetCurrentActorInfo()->IsLocallyControlled())
	{
		SpawnPreview(GetCurrentActorInfo());
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

	SpawnedTurretPreviewActor = GetWorld()->SpawnActor<ADeployableTurretPreview>(TurretPreviewActorClass, GetAvatarActorFromActorInfo()->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

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
	PlaceTurretMontageTask->OnBlendOut.AddDynamic(this, &UThrowable_Enforcer::ConfirmThrow);
	
	// If the turret is placed in a valid location, play the animation
	if (SpawnedTurretPreviewActor->bCanPlace)
	{
		SpawnedTurretPreviewActor->bShouldUpdatePosition = false;
		PlaceTurretMontageTask->ReadyForActivation();
	}
	else
	{
		// Re-create the task so the player can try placing again
		WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
		WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::PlayPlaceTurretAnimation);
		WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::PlayPlaceTurretAnimation);
		WaitConfirm->ReadyForActivation();
	}
}

void UThrowable_Enforcer::ConfirmThrow()
{
	if (!SpawnedTurretPreviewActor || !SpawnedTurretPreviewActor->bCanPlace)
        {
            // Re-create the task so the player can try placing again
            WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
            WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmThrow);
            WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelThrow);
            WaitConfirm->ReadyForActivation();
            return;
        }
	
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	PlaceTurret();

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UThrowable_Enforcer::PlaceTurret()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;
	
	// Trace to the middle of the screen (crosshair)
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
		
		FVector End = Start + CrosshairWorldDirection * 500;
		
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Avatar);
		
		FVector SpawnLocation = Start;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		
		// The turret will spawn rotated towards the crosshair's world direction rotation
		FRotator SpawnRotation = CrosshairWorldDirection.Rotation();
		SpawnRotation.Yaw += 0.f;
		SpawnRotation.Pitch = 0.f;
		SpawnRotation.Roll = 0.f;
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Avatar;
		SpawnParams.Instigator = Cast<APawn>(Avatar);

		// A server RPC is used to handle spawning the turret
		UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		if (ASC)
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
