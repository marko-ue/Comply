// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Enforcer/Throwable_Enforcer.h"

#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Actors/DeployableTurret/DeployableTurret.h"
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

	// The bound function will be called when the OnConfirm event is received from where the input is handled (player class)
	// It will only be received if the input is pressed while the ability is already active (preview spawned)
	UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmThrow);
	WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelThrow);
	WaitConfirm->ReadyForActivation();
}

void UThrowable_Enforcer::SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo)
{
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

void UThrowable_Enforcer::ConfirmThrow()
{
	if (!SpawnedTurretPreviewActor || !SpawnedTurretPreviewActor->bCanPlace)
        {
            // Re-create the task so the player can try placing again
            UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
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

	// Destroy the preview actor now, as the actual turret is already placed
	if (SpawnedTurretPreviewActor)
	{
		SpawnedTurretPreviewActor->Destroy();
		SpawnedTurretPreviewActor = nullptr;
	}

	PlaceTurret();

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
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
		
		const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

		if (HasAuthority(&ActivationInfo))
		{
			APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
			
			FTransform SpawnTransform = FTransform(
			SpawnRotation,
			SpawnLocation
		);
			
			ADeployableTurret* Turret = GetWorld()->SpawnActorDeferred<ADeployableTurret>(TurretActorClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			if (Turret)
			{
				// TODO: Turn into a scalable float to make it upgradeable
				Turret->Damage = 10.f;
				Turret->SetLifeSpan(TurretLifetime);
				Turret->SourceASC = GetAbilitySystemComponentFromActorInfo();
				Turret->DamageEffectClass = DamageEffectClass;
				Turret->DamageTypeTag = DamageType;
				
				UGameplayStatics::FinishSpawningActor(Turret, SpawnTransform);
			}
		}
	}
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
