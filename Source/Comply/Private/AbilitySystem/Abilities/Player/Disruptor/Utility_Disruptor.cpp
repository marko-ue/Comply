// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Utility_Disruptor.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "Actors/BuffTotem/BuffTotemPreview.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UUtility_Disruptor::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Use();
}

void UUtility_Disruptor::Use()
{
	// The preview for the buff totem will be shown only on the owning client
	if (GetCurrentActorInfo()->IsLocallyControlled())
	{
		SpawnPreview(GetCurrentActorInfo());
	}

	// The bound function will be called when the OnConfirm event is received from where the input is handled (player class)
	// It will only be received if the input is pressed while the ability is already active (preview spawned)
	UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmPlacement);
	WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelPlacement);
	WaitConfirm->ReadyForActivation();
}

void UUtility_Disruptor::SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo)
{
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	SpawnedBuffTotemPreviewActor = GetWorld()->SpawnActor<ABuffTotemPreview>(BuffTotemPreviewActorClass, GetAvatarActorFromActorInfo()->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

	if (SpawnedBuffTotemPreviewActor)
	{
		SpawnedBuffTotemPreviewActor->InitPreviewData(Cast<ACharacter>(ActorInfo->AvatarActor.Get()));
	}
}

void UUtility_Disruptor::ConfirmPlacement()
{
	if (!SpawnedBuffTotemPreviewActor || !SpawnedBuffTotemPreviewActor->bCanPlace)
	{
		// Re-create the task so the player can try placing again
		UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
		WaitConfirm->OnConfirm.AddDynamic(this, &ThisClass::ConfirmPlacement);
		WaitConfirm->OnCancel.AddDynamic(this, &ThisClass::CancelPlacement);
		WaitConfirm->ReadyForActivation();
		return;
	}

	// Destroy the preview actor now, as the actual buff totem is already placed
	if (SpawnedBuffTotemPreviewActor)
	{
		SpawnedBuffTotemPreviewActor->Destroy();
		SpawnedBuffTotemPreviewActor = nullptr;
	}

	UAbilityTask_PlayMontageAndWait* PlaceBuffTotemMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlaceBuffTotemMontage, 1.f, NAME_None, true);
	PlaceBuffTotemMontageTask->OnCompleted.AddDynamic(this, &UUtility_Disruptor::TraceAndSpawnBuffTotem);
	PlaceBuffTotemMontageTask->ReadyForActivation();
}

void UUtility_Disruptor::TraceAndSpawnBuffTotem()
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
		
		// The buff totem will spawn rotated towards the crosshair's world direction rotation
		FRotator SpawnRotation = CrosshairWorldDirection.Rotation();
		SpawnRotation.Yaw += 0.f;
		SpawnRotation.Pitch = 0.f;
		SpawnRotation.Roll = 0.f;
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Avatar;
		SpawnParams.Instigator = Cast<APawn>(Avatar);
		
		const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();

		// A server RPC is used to handle spawning the buff totem
		UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		if (ASC)
		{
			ASC->Server_PlaceBuffTotem(GetCurrentAbilitySpecHandle(), SpawnLocation, BuffTotemLifetime);
		}
	}
	
	// Automatically equip the primary ability once the buff totem is thrown, as the player should not be able to equip the buff totem while it's on cooldown
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(
				FGameplayTagContainer(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary));
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UUtility_Disruptor::CancelPlacement()
{
	if (SpawnedBuffTotemPreviewActor)
	{
		SpawnedBuffTotemPreviewActor->Destroy();
		SpawnedBuffTotemPreviewActor = nullptr;
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UUtility_Disruptor::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SpawnedBuffTotemPreviewActor)
	{
		SpawnedBuffTotemPreviewActor->Destroy();
		SpawnedBuffTotemPreviewActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
