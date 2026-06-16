// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/DecoyGrenade/DecoyGrenade.h"
#include "Actors/DecoyGrenade/DecoyGrenadePreview.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interface/Player/WeaponInterface.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Disruptor::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	bool bFound = false;
	float Charges = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(
		UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute(), bFound);
	if (Charges <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_ThrowableThrowing);
	
	SpawnPreview();
}

void UThrowable_Disruptor::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	SafeRemoveThrowingTag();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UThrowable_Disruptor::SpawnPreview()
{
	// For this throwable, we manually handle adding and removing the firing tag
	// Because of the delay of the ability actually finishing only after releasing primary input
	// ^ This results in the player being able to freely rotate the camera before the grenade is thrown causing weird looking behavior
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_Firing);
	
	// Play the prepare section of the montage first
	PrepareDecoyMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ThrowDecoyMontage, 1.f, "Prepare", true);
	PrepareDecoyMontageTask->ReadyForActivation();
	
	FGameplayCueParameters CueParams;
	CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	UGameplayCueManager::ExecuteGameplayCue_NonReplicated(
		GetAvatarActorFromActorInfo(), ComplyTags::GameplayCues::PullGrenadePin, CueParams);
	
	// Input is confirmed when the primary input is released
	UAbilityTask_WaitConfirmCancel* WaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirm->OnConfirm.AddDynamic(this, &UThrowable_Disruptor::ConfirmThrow);
	WaitConfirm->ReadyForActivation();
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	FTransform SpawnTransform = FTransform(
		InstigatorPawn->GetActorRotation(),
		InstigatorPawn->GetActorLocation()
	);
	
	SpawnedDecoyGrenadePreviewActor = GetWorld()->SpawnActorDeferred<ADecoyGrenadePreview>(
		DecoyGrenadePreviewActorClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	// Information needed to predict the path correctly
	SpawnedDecoyGrenadePreviewActor->ActorsToIgnore.Add(InstigatorPawn);
	SpawnedDecoyGrenadePreviewActor->OwningPawn = InstigatorPawn;
	SpawnedDecoyGrenadePreviewActor->ThrowSpeed = ThrowSpeed;
	
	UGameplayStatics::FinishSpawningActor(SpawnedDecoyGrenadePreviewActor, SpawnTransform);
}

void UThrowable_Disruptor::ThrowOnServer(FVector LaunchVelocity, FVector SpawnPosition)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostEffectClass, 1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	AActor* Avatar = GetAvatarActorFromActorInfo();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	EquipWeaponBasedOnCharges(WeaponOwner, GetAbilitySystemComponentFromActorInfo());
	
	const FTransform SpawnTransform(GetAvatarActorFromActorInfo()->GetActorRotation(), SpawnPosition);
	
	ADecoyGrenade* DecoyGrenade = GetWorld()->SpawnActorDeferred<ADecoyGrenade>(DecoyGrenadeActorClass, SpawnTransform, GetOwningActorFromActorInfo(), GetAvatarActorFromActorInfo()->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (DecoyGrenade)
	{
		DecoyGrenade->PullRadius = PullRadius;
		DecoyGrenade->DecoyGrenadeLifetime = DecoyGrenadeLifetime;
		DecoyGrenade->SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		DecoyGrenade->DamageEffectClass = DamageEffectClass;
		DecoyGrenade->DamageTypeTag = DamageType;
		
		// Clamp to the throw speed to prevent cheating by the client passing in higher values
		FVector SafeLaunchVelocity = LaunchVelocity.GetClampedToMaxSize(ThrowSpeed);
		DecoyGrenade->ProjectileMovementComp->Velocity = SafeLaunchVelocity;

		UGameplayStatics::FinishSpawningActor(DecoyGrenade, SpawnTransform);
	}
}

void UThrowable_Disruptor::ConfirmThrow()
{
	// End the prepare grenade task now, as the throw is confirmed
	if (PrepareDecoyMontageTask)
	{
		PrepareDecoyMontageTask->EndTask();
		PrepareDecoyMontageTask = nullptr;
	}
	
	// Now play the throw section of the montage
	UAbilityTask_PlayMontageAndWait* ThrowTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ThrowDecoyMontage, 1.f, "Throw", true);
	ThrowTask->OnCompleted.AddDynamic(this, &UThrowable_Disruptor::OnThrowMontageCompleted);
	ThrowTask->OnBlendOut.AddDynamic(this, &UThrowable_Disruptor::OnThrowMontageCompleted);
	ThrowTask->ReadyForActivation();
}

void UThrowable_Disruptor::EquipWeaponBasedOnCharges(IWeaponInterface* WeaponOwner, UAbilitySystemComponent* ASC)
{
	const UWeaponAttributeSet* WeaponAS = ASC->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	float GrenadeCurrentCharges = ASC->GetGameplayAttributeValue(
		UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute(), bFound);

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
			// Add this tag so the equip throwable ability can't be activated anymore
			ASC->AddReplicatedLooseGameplayTag(ComplyTags::States::State_NoThrowables);
			
			WeaponOwner->ClearEquippedWeapon();
			
			FGameplayTagContainer Tags;
			Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary);
			ASC->TryActivateAbilitiesByTag(Tags);
		}
	}
}

// Only throw the grenade and end the ability after the throw section of the animation finishes
void UThrowable_Disruptor::OnThrowMontageCompleted()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	
	if (SpawnedDecoyGrenadePreviewActor) SpawnedDecoyGrenadePreviewActor->Destroy(); SpawnedDecoyGrenadePreviewActor = nullptr;
	
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_Firing);
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);

	UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ASC)
	{
		AActor* Owner = GetOwningActorFromActorInfo();
	
		if (!Avatar || !Owner) return;
	
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
		if (!bScreenToWorld) return;
	
		const FVector LaunchVelocity = CrosshairWorldDirection * ThrowSpeed;
		const FVector SpawnPosition = Avatar->GetActorLocation() + FVector(0.f, 0.f, 60.f) + CrosshairWorldDirection * 40.f;

		// Spawn the grenade through the RPC if not on server and execute the throw cue
		if (!ASC->IsOwnerActorAuthoritative())
		{
			APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
			ASC->Server_ThrowDecoyGrenade(GetCurrentAbilitySpecHandle(), SpawnPosition, InstigatorPawn->GetActorRotation(), LaunchVelocity);
			
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

// This function is overridden so ability costs can be handled manually
// The charge would usually get consumed when the input is pressed, doing it manually allows the player to use all charges
bool UThrowable_Disruptor::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	return true;
}

void UThrowable_Disruptor::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	SafeRemoveThrowingTag();
	
	if (SpawnedDecoyGrenadePreviewActor) SpawnedDecoyGrenadePreviewActor->Destroy();
}
