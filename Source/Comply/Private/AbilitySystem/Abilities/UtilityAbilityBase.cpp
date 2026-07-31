// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/UtilityAbilityBase.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Actors/AbilityActors/BuffTotem/BuffTotemPreview.h"
#include "GameFramework/Character.h"


void UUtilityAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (bDoesAbilitySpawnActor)
	{
		// The preview for the utility will be shown only on the owning client
		if (GetCurrentActorInfo()->IsLocallyControlled())
		{
			SpawnPreview();
		}

		// The bound function will be called when the OnConfirm event is received from where the input is handled (player class)
		// It will only be received if the input is pressed while the ability is already active (preview spawned)
		UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
		WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmPlacement);
		WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelPlacement);
		WaitConfirm->ReadyForActivation();
	}
}

void UUtilityAbilityBase::SpawnPreview()
{
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	SpawnedUtilityPreviewActor = GetWorld()->SpawnActor<AUtilityPreviewBase>(
		UtilityPreviewActorClass, 
		GetAvatarActorFromActorInfo()->GetActorLocation(), 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	if (SpawnedUtilityPreviewActor)
	{
		SpawnedUtilityPreviewActor->InitPreviewData(Cast<ACharacter>(GetCurrentActorInfo()->AvatarActor.Get()));
	}
}

void UUtilityAbilityBase::ConfirmPlacement()
{
	// Destroy the preview actor now, as the actual buff totem is already placed
	if (SpawnedUtilityPreviewActor)
	{
		CachedPlaceLocation = SpawnedUtilityPreviewActor->GetActorLocation();
		SpawnedUtilityPreviewActor->Destroy();
		SpawnedUtilityPreviewActor = nullptr;
	}

	UAbilityTask_PlayMontageAndWait* PlaceUtilityMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlaceUtilityMontage, 1.f, NAME_None, true);
	PlaceUtilityMontageTask->OnCompleted.AddDynamic(this, &ThisClass::TraceAndSpawn);
	PlaceUtilityMontageTask->ReadyForActivation();
}

void UUtilityAbilityBase::TraceAndSpawn()
{
	
}

void UUtilityAbilityBase::CancelPlacement()
{
	if (SpawnedUtilityPreviewActor)
	{
		SpawnedUtilityPreviewActor->Destroy();
		SpawnedUtilityPreviewActor = nullptr;
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UUtilityAbilityBase::CancelAbility(const FGameplayAbilitySpecHandle Handle,
										const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
										bool bReplicateCancelAbility)
{
	// Only cancel the ability if the actual shield is not spawned. It's only possible to cancel when the preview is showing
	// This prevents unintended behavior where the player would remove their placed shield
	if (SpawnedUtilityPreviewActor)
	{
		SpawnedUtilityPreviewActor->Destroy();
		SpawnedUtilityPreviewActor = nullptr;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UUtilityAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SpawnedUtilityPreviewActor)
	{
		SpawnedUtilityPreviewActor->Destroy();
		SpawnedUtilityPreviewActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
