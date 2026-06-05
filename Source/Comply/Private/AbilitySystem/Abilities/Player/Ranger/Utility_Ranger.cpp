// Copyright © 2026 Marko. All rights reserved.

#include "AbilitySystem/Abilities/Player/Ranger/Utility_Ranger.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyTags.h"
#include "Actors/ShieldDome/ShieldDome.h"
#include "Actors/ShieldDome/ShieldDomePreview.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UUtility_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Use();
}

void UUtility_Ranger::Use()
{
	// The preview for the shield will be shown only on the owning client
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

void UUtility_Ranger::SpawnPreview(const FGameplayAbilityActorInfo* ActorInfo)
{
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FVector Forward = Avatar->GetActorForwardVector();
	FVector Start = Avatar->GetActorLocation() + Forward * 150.f;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	SpawnedShieldPreviewActor = GetWorld()->SpawnActor<AShieldDomePreview>(ShieldPreviewActorClass, GetAvatarActorFromActorInfo()->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

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

	// Destroy the preview actor now, as the actual shield is already placed
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->Destroy();
		SpawnedShieldPreviewActor = nullptr;
	}
	
	UAbilityTask_PlayMontageAndWait* PlaceShieldMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, PlaceShieldMontage, 1.f, NAME_None, true);
	PlaceShieldMontageTask->OnCompleted.AddDynamic(this, &UUtility_Ranger::TraceAndSpawnShield);
	PlaceShieldMontageTask->ReadyForActivation();
}

void UUtility_Ranger::TraceAndSpawnShield()
{
	AActor* Avatar = GetCurrentActorInfo()->AvatarActor.Get();
	if (!Avatar) return;

	FVector Forward = Avatar->GetActorForwardVector();
	FVector Start = Avatar->GetActorLocation() + Forward * 150.f;

	FHitResult Hit;
	FVector End = Start - FVector(0.f, 0.f, 500.f);

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
		AShieldDome* Shield = GetWorld()->SpawnActorDeferred<AShieldDome>(
			ShieldActorClass, 
			FTransform(FRotator::ZeroRotator, SpawnLocation),
			Avatar,
			Cast<APawn>(Avatar),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
    
		if (Shield)
		{
			Shield->SourceASC = GetAbilitySystemComponentFromActorInfo();
			UGameplayStatics::FinishSpawningActor(Shield, FTransform(FRotator::ZeroRotator, SpawnLocation));
			Shield->SetLifeSpan(ShieldLifetime);
		}
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = SpawnLocation;
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::ShieldPlaced, CueParams);
	
	// Automatically equip the primary ability once the shield is thrown, as the player should not be able to equip the shield while it's on cooldown
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(
				FGameplayTagContainer(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary));
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UUtility_Ranger::CancelPlacement()
{
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->Destroy();
		SpawnedShieldPreviewActor = nullptr;
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UUtility_Ranger::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	// Only cancel the ability if the actual shield is not spawned. It's only possible to cancel when the preview is showing
	// This prevents unintended behavior where the player would remove their placed shield
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->Destroy();
		SpawnedShieldPreviewActor = nullptr;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UUtility_Ranger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SpawnedShieldPreviewActor)
	{
		SpawnedShieldPreviewActor->Destroy();
		SpawnedShieldPreviewActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
