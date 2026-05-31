// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/DecoyGrenade/DecoyGrenadePreview.h"
#include "Interface/Player/WeaponInterface.h"
#include "Kismet/GameplayStatics.h"


void UThrowable_Disruptor::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	SpawnPreview();
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

void UThrowable_Disruptor::OnThrowMontageCompleted()
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostEffectClass, 1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	if (SpawnedDecoyGrenadePreviewActor) SpawnedDecoyGrenadePreviewActor->Destroy();
	
	// A server RPC is used to handle spawning the decoy grenade
	UComplyAbilitySystemComponent* ASC = Cast<UComplyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (ASC)
	{
		AActor* Owner = GetOwningActorFromActorInfo();
		AActor* Avatar = GetAvatarActorFromActorInfo();

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

		APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
		ASC->Server_ThrowDecoyGrenade(GetCurrentAbilitySpecHandle(), SpawnPosition, InstigatorPawn->GetActorRotation(), LaunchVelocity);
	}
	
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_Firing);
	
	AActor* Avatar = GetAvatarActorFromActorInfo();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	
	const UWeaponAttributeSet* WeaponAS = GetAbilitySystemComponentFromActorInfo()->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	float DecoyGrenadeCurrentCharges = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(
		UWeaponAttributeSet::GetDecoyGrenadeCurrentChargesAttribute(), bFound);
	
	// Clear equip slot and equip the throwable again to simulate grabbing another grenade from the inventory
	if (WeaponAS && DecoyGrenadeCurrentCharges > 0.f)
	{
		if (WeaponOwner)
		{
			WeaponOwner->ClearEquippedWeapon();
			
			FGameplayTagContainer Tags;
			Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Throwable);
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(Tags);
		}
	}
	else // If there are no more grenades, equip the primary
	{
		if (WeaponOwner)
		{
			// Add this tag so the equip throwable ability can't be activated anymore
			GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_NoThrowables);
			
			WeaponOwner->ClearEquippedWeapon();
			
			FGameplayTagContainer Tags;
			Tags.AddTag(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary);
			GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(Tags);
		}
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
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
	
	if (SpawnedDecoyGrenadePreviewActor) SpawnedDecoyGrenadePreviewActor->Destroy();
}
