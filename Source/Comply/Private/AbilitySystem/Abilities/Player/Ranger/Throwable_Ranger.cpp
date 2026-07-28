// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/AbilityActors/PlasmaGrenade/PlasmaGrenade.h"
#include "Actors/AbilityActors/PlasmaGrenade/PlasmaGrenadePreview.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UThrowable_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
	// Hard gate - don't even start if no charges
	bool bFound = false;
	const float Charges = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(
		UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute(), bFound
	);
	if (Charges <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_ThrowableThrowing);
	SpawnPreview();
}

void UThrowable_Ranger::SpawnPreview()
{
	// For this throwable, we manually handle adding and removing the firing tag
	// because of the delay of the ability actually finishing only after releasing primary input
	// ^ This results in the player being able to freely rotate the camera before the grenade is thrown causing weird looking behavior
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_Firing);
	
	// Play the prepare section of the montage first
	PrepareGrenadeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ThrowGrenadeMontage, 1.f, "Prepare", true);
	PrepareGrenadeMontageTask->ReadyForActivation();
	
	FGameplayCueParameters CueParams;
	CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
		GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::PullGrenadePin, CueParams);
	
	// Input is confirmed when the primary input is released
	UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &UThrowable_Ranger::ConfirmThrow);
	WaitConfirm->ReadyForActivation();
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	FTransform SpawnTransform = FTransform(
		InstigatorPawn->GetActorRotation(),
		InstigatorPawn->GetActorLocation()
	);
	
	SpawnedGrenadePreviewActor = GetWorld()->SpawnActorDeferred<APlasmaGrenadePreview>(
		GrenadePreviewActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		InstigatorPawn,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	// Information needed to predict the path correctly
	SpawnedGrenadePreviewActor->ActorsToIgnore.Add(InstigatorPawn);
	SpawnedGrenadePreviewActor->OwningPawn = InstigatorPawn;
	SpawnedGrenadePreviewActor->ThrowSpeed = ThrowSpeed;
	
	UGameplayStatics::FinishSpawningActor(SpawnedGrenadePreviewActor, SpawnTransform);
}

void UThrowable_Ranger::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnPosition);
	
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostEffectClass, 1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	AActor* Avatar = GetAvatarActorFromActorInfo();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	EquipWeaponBasedOnCharges(WeaponOwner, GetAbilitySystemComponentFromActorInfo());
	
	APlasmaGrenade* Grenade = GetWorld()->SpawnActorDeferred<APlasmaGrenade>(
		GrenadeActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		GetAvatarActorFromActorInfo()->GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (Grenade)
	{
		Grenade->ExplosionRadius = ExplosionRadius;
		Grenade->MaxDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		Grenade->SourceASC = GetAbilitySystemComponentFromActorInfo();
		Grenade->DamageEffectClass = DamageEffectClass;
		Grenade->DamageTypeTag = DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = LaunchVelocity.GetClampedToMaxSize(ThrowSpeed);
		Grenade->LaunchVelocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(Grenade, SpawnTransform);
	}
}

void UThrowable_Ranger::ConfirmThrow()
{
	// End the prepare grenade task now, as the throw is confirmed
	if (PrepareGrenadeMontageTask)
	{
		PrepareGrenadeMontageTask->EndTask();
		PrepareGrenadeMontageTask = nullptr;
	}
	
	// Now play the throw section of the montage
	UAbilityTask_PlayMontageAndWait* ThrowTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ThrowGrenadeMontage, 1.f, "Throw", true);
	ThrowTask->OnCompleted.AddDynamic(this, &UThrowable_Ranger::OnThrowMontageCompleted);
	ThrowTask->OnBlendOut.AddDynamic(this, &UThrowable_Ranger::OnThrowMontageCompleted);
	ThrowTask->ReadyForActivation();
}

// Only throw the grenade and end the ability after the throw section of the animation finishes
void UThrowable_Ranger::OnThrowMontageCompleted()
{
	const AActor* Avatar = GetAvatarActorFromActorInfo();
	
	if (SpawnedGrenadePreviewActor) SpawnedGrenadePreviewActor->Destroy(); SpawnedGrenadePreviewActor = nullptr;
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	
	if (UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		const AActor* Owner = GetOwningActorFromActorInfo();
		if (!Avatar || !Owner) return;
		
		FVector TraceStart, TraceEnd, TraceDirection;
		if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, Avatar, 0.f, TraceStart, TraceEnd, TraceDirection)) return;
		
		const FVector LaunchVelocity = TraceDirection * ThrowSpeed;
		const FVector SpawnPosition = Avatar->GetActorLocation() + FVector(0.f, 0.f, 60.f) + TraceDirection * 40.f;

		// Spawn the grenade through the RPC if not on server and execute the throw cue
		if (!ASC->IsOwnerActorAuthoritative())
		{
			const APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
			ASC->Server_ThrowPlasmaGrenade(GetCurrentAbilitySpecHandle(), SpawnPosition, InstigatorPawn->GetActorRotation(), LaunchVelocity);
			
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

void UThrowable_Ranger::EquipWeaponBasedOnCharges(IWeaponInterface* WeaponOwner, UAbilitySystemComponent* ASC) const
{
	const UWeaponAttributeSet* WeaponAS = ASC->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	const float GrenadeCurrentCharges = ASC->GetGameplayAttributeValue(
		UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute(), bFound
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
			const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(NoThrowablesEffectClass, 1.f);
			GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			
			WeaponOwner->ClearEquippedWeapon();
			
			FGameplayTagContainer Tags;
			Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary);
			ASC->TryActivateAbilitiesByTag(Tags);
		}
	}
}

// This function is overridden so ability costs can be handled manually
// The charge would usually get consumed when the input is pressed, doing it manually allows the player to use all charges
bool UThrowable_Ranger::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	return true;
}

void UThrowable_Ranger::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	SafeRemoveThrowingTag();
	
	if (SpawnedGrenadePreviewActor) SpawnedGrenadePreviewActor->Destroy();
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UThrowable_Ranger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	SafeRemoveThrowingTag();
	
	// Remove both the loose tag and any GE granting it
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_Firing);
	
	FGameplayTagContainer TagsToRemove;
	TagsToRemove.AddTag(ComplyTags::States::State_Firing);
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(TagsToRemove);
	
	if (SpawnedGrenadePreviewActor)
	{
		SpawnedGrenadePreviewActor->Destroy();
		SpawnedGrenadePreviewActor = nullptr;
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
