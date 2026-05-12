// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Utility_Ranger.h"
#include "Actors/ShieldDomePreview.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GameFramework/Character.h"


void UUtility_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	Use();
}

void UUtility_Ranger::Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Ranger utility activated"));
	
	// The preview for the shield will be shown only on the owning client
	if (GetCurrentActorInfo()->IsLocallyControlled())
	{
		SpawnPreview(GetCurrentActorInfo());
	}
	
	// The bound function will be called when the OnConfirm event is received from where the input is handled (player class)
	// It will only be received if the input is pressed while the ability is already active (preview spawned)
	UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &UUtility_Ranger::ConfirmPlacement);
	WaitConfirm->OnCancel.AddDynamic(this, &UUtility_Ranger::CancelPlacement);
	WaitConfirm->ReadyForActivation();
}

void UUtility_Ranger::OnShieldExpired()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UUtility_Ranger::SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo)
{
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FVector Forward = Avatar->GetActorForwardVector();
	FVector Start = Avatar->GetActorLocation() + Forward * 150.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	FVector SpawnLocation = Start;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);
	
	SpawnedShieldPreviewActor = GetWorld()->SpawnActor<AShieldDomePreview>(ShieldPreviewActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->InitPreviewData(Cast<ACharacter>(ActorInfo->AvatarActor.Get()));
	}
}

void UUtility_Ranger::ConfirmPlacement()
{
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
	
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ShieldLifetime);
	WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnShieldExpired);
	WaitDelayTask->ReadyForActivation();
	
	// Destroy the preview actor now, as the actual shield is already placed
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->Destroy();
	}
	
	TraceAndSpawnShield();
}

void UUtility_Ranger::CancelPlacement()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UUtility_Ranger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->Destroy();
		SpawnedShieldPreviewActor = nullptr;
	}
	
	if (SpawnedShieldActor.IsValid())
	{
		SpawnedShieldActor->Destroy();
		SpawnedShieldActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UUtility_Ranger::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	// Only cancel the ability if the actual shield is not spawned. It's only possible to cancel when the preview is showing
	// This prevents unintended behavior where the player would remove their placed shield
	if (SpawnedShieldPreviewActor && !SpawnedShieldActor.IsValid())
	{
		SpawnedShieldPreviewActor->Destroy();
		SpawnedShieldPreviewActor = nullptr;
		
		Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	}
}

void UUtility_Ranger::TraceAndSpawnShield()
{
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FVector Forward = Avatar->GetActorForwardVector();
	FVector Start = Avatar->GetActorLocation() + Forward * 150.f;

	// Trace to ground
	FHitResult Hit;
	FVector End = Start - FVector(0, 0, 500.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	FVector SpawnLocation = Start;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		SpawnLocation = Hit.ImpactPoint;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	const FGameplayAbilityActivationInfo ActivationInfo = GetCurrentActivationInfo();
	if (HasAuthority(&ActivationInfo))
	{
		SpawnedShieldActor = GetWorld()->SpawnActor<AActor>(ShieldActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}
