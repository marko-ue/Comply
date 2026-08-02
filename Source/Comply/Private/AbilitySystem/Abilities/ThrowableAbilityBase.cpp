// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "AbilitySystem/Data/Player/Grenades/ComplyGrenadeData.h"
#include "Actors/AbilityActors/ThrowablePreviewBase.h"
#include "Interface/Player/WeaponInterface.h"
#include "Kismet/GameplayStatics.h"

void UThrowableAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	checkf(GrenadeData, TEXT("GrenadeData not set on %s"), *GetName());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (GrenadeData != nullptr) // If not a grenade throwable
	{
		// Hard gate - don't even start if no charges
		bool bFound = false;
		const float Charges = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(
			GetThrowableCurrentChargesAttribute(), bFound
		);
		if (Charges <= 0.f)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}

		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_ThrowableThrowing);
	
		SpawnPreview();
	}
}

void UThrowableAbilityBase::SpawnPreview()
{
	// For this throwable, we manually handle adding and removing the firing tag
	// Because of the delay of the ability actually finishing only after releasing primary input
	// ^ This results in the player being able to freely rotate the camera before the grenade is thrown causing weird looking behavior
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_Firing);
	
	// Play the prepare section of the montage first
	PrepareThrowMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, GrenadeData->ThrowMontage, 1.f, "Prepare", true);
	PrepareThrowMontageTask->ReadyForActivation();
	
	FGameplayCueParameters CueParams;
	CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
		GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::PullGrenadePin, CueParams
	);
	
	// Input is confirmed when the primary input is released
	UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &UThrowableAbilityBase::ConfirmThrow);
	WaitConfirm->ReadyForActivation();
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	FTransform SpawnTransform = FTransform(
		InstigatorPawn->GetActorRotation(),
		InstigatorPawn->GetActorLocation()
	);
	
	SpawnedThrowablePreviewActor = GetWorld()->SpawnActorDeferred<AThrowablePreviewBase>(
		GrenadeData->PreviewGrenadeActor,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (SpawnedThrowablePreviewActor)
	{
		// Information needed to predict the path correctly
		SpawnedThrowablePreviewActor->ActorsToIgnore.Add(InstigatorPawn);
		SpawnedThrowablePreviewActor->OwningPawn = InstigatorPawn;
		SpawnedThrowablePreviewActor->ThrowSpeed = GrenadeData->ThrowSpeed;
	}
	
	UGameplayStatics::FinishSpawningActor(SpawnedThrowablePreviewActor, SpawnTransform);
}

void UThrowableAbilityBase::ConfirmThrow()
{
	// End the prepare grenade task now, as the throw is confirmed
	if (PrepareThrowMontageTask)
	{
		PrepareThrowMontageTask->EndTask();
		PrepareThrowMontageTask = nullptr;
	}
	
	// Now play the throw section of the montage
	UAbilityTask_PlayMontageAndWait* ThrowTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, GrenadeData->ThrowMontage, 1.f, "Throw", true);
	ThrowTask->OnCompleted.AddDynamic(this, &UThrowableAbilityBase::OnThrowMontageCompleted);
	ThrowTask->OnBlendOut.AddDynamic(this, &UThrowableAbilityBase::OnThrowMontageCompleted);
	ThrowTask->ReadyForActivation();
}

void UThrowableAbilityBase::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GrenadeData->CostEffectClass, 1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	AActor* Avatar = GetAvatarActorFromActorInfo();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	EquipWeaponBasedOnCharges(WeaponOwner, GetAbilitySystemComponentFromActorInfo());
}

void UThrowableAbilityBase::CallThrowRPC(UComplyAbilitySystemComponent* ASC, FVector LaunchVelocity,
	FVector SpawnPosition)
{
	
}

void UThrowableAbilityBase::OnThrowMontageCompleted()
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	
	if (SpawnedThrowablePreviewActor) SpawnedThrowablePreviewActor->Destroy(); SpawnedThrowablePreviewActor = nullptr;
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	
	if (UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		const AActor* Owner = GetOwningActorFromActorInfo();
	
		if (!Avatar || !Owner) return;
	
		FVector TraceStart, TraceEnd, TraceDirection;
		if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, 0.f, TraceStart, TraceEnd, TraceDirection)) return;
	
		const FVector LaunchVelocity = TraceDirection * GrenadeData->ThrowSpeed;
		const FVector SpawnPosition = Avatar->GetActorLocation() + FVector(0.f, 0.f, 60.f) + TraceDirection * 40.f;

		// Spawn the grenade through the RPC if not on server and execute the throw cue
		if (!ASC->IsOwnerActorAuthoritative())
		{
			CallThrowRPC(ASC, LaunchVelocity, SpawnPosition);
			
			FGameplayCueParameters CueParams;
			CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::ThrowGrenade, CueParams);
		}
		else if (GetCurrentActorInfo()->IsLocallyControlled()) // On the server path, checking if locally controlled to prevent double spawn
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
			UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::ThrowGrenade, CueParams);
			
			ThrowOnServer(LaunchVelocity, SpawnPosition);
		}
	}
}

void UThrowableAbilityBase::EquipWeaponBasedOnCharges(IWeaponInterface* WeaponOwner, UAbilitySystemComponent* ASC)
{
	const UWeaponAttributeSet* WeaponAS = ASC->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	const float GrenadeCurrentCharges = ASC->GetGameplayAttributeValue(
		GetThrowableCurrentChargesAttribute(), bFound
	);

	// Clear equip slot and equip the throwable again to simulate grabbing another grenade from the inventory
	if (WeaponAS && GrenadeCurrentCharges > 0.f)
	{
		if (WeaponOwner)
		{
			WeaponOwner->ClearEquippedWeapon();
			
			FGameplayTagContainer Tags;
			Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Throwable);
			ASC->TryActivateAbilitiesByTag(Tags);
		}
	}
	else // If there are no more grenades, equip the primary
	{
		if (WeaponOwner)
		{
			// GE that applies the NoThrowables tag so grenades can't be equipped when there's no throwables
			const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GrenadeData->NoThrowablesEffectClass, 1.f);
			GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			
			WeaponOwner->ClearEquippedWeapon();
			
			FGameplayTagContainer Tags;
			Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary);
			ASC->TryActivateAbilitiesByTag(Tags);
		}
	}
}

FGameplayAttribute UThrowableAbilityBase::GetThrowableCurrentChargesAttribute()
{
	return FGameplayAttribute();
}

void UThrowableAbilityBase::SafeRemoveThrowingTag() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	// Only remove if we actually own a count of this tag
	if (ASC->GetTagCount(ComplyTags::States::State_ThrowableThrowing) > 0)
	{
		ASC->RemoveLooseGameplayTag(ComplyTags::States::State_ThrowableThrowing);
	}
}

void UThrowableAbilityBase::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	SafeRemoveThrowingTag();
	
	if (SpawnedThrowablePreviewActor) SpawnedThrowablePreviewActor->Destroy();
}

bool UThrowableAbilityBase::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	return true;
}

void UThrowableAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	SafeRemoveThrowingTag();
	
	// Remove both the loose tag and any GE granting it
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_Firing);
	
	FGameplayTagContainer TagsToRemove;
	TagsToRemove.AddTag(ComplyTags::States::State_Firing);
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
	
	if (SpawnedThrowablePreviewActor)
	{
		SpawnedThrowablePreviewActor->Destroy();
		SpawnedThrowablePreviewActor = nullptr;
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
