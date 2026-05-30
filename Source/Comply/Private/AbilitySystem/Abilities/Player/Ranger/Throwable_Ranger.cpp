// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/PlasmaGrenade/PlasmaGrenade.h"
#include "Actors/PlasmaGrenade/PlasmaGrenadePreview.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UThrowable_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	SpawnPreview();
}

void UThrowable_Ranger::SpawnPreview()
{
	// For this throwable, we manually handle adding and removing the firing tag
	// Because of the delay of the ability actually finishing only after releasing primary input
	// ^ This results in the player being able to freely rotate the camera before the grenade is thrown causing weird looking behavior
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(ComplyTags::States::State_Firing);
	
	// Play the prepare section of the montage first
	PrepareGrenadeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, ThrowGrenadeMontage, 1.f, "Prepare", true);
	PrepareGrenadeMontageTask->ReadyForActivation();
	
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
		GrenadePreviewActorClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	// Information needed to predict the path correctly
	SpawnedGrenadePreviewActor->ActorsToIgnore.Add(InstigatorPawn);
	SpawnedGrenadePreviewActor->OwningPawn = InstigatorPawn;
	SpawnedGrenadePreviewActor->ThrowSpeed = ThrowSpeed;
	
	UGameplayStatics::FinishSpawningActor(SpawnedGrenadePreviewActor, SpawnTransform);
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
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostEffectClass, 1.f);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	if (SpawnedGrenadePreviewActor) SpawnedGrenadePreviewActor->Destroy();
	
	// A server RPC is used to handle spawning the grenade
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
		ASC->Server_ThrowPlasmaGrenade(GetCurrentAbilitySpecHandle(), InstigatorPawn->GetActorLocation(), InstigatorPawn->GetActorRotation(), LaunchVelocity);
	}
	
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(ComplyTags::States::State_Firing);
	
	AActor* Avatar = GetAvatarActorFromActorInfo();
	IWeaponInterface* WeaponOwner = Cast<IWeaponInterface>(Avatar);
	
	const UWeaponAttributeSet* WeaponAS = GetAbilitySystemComponentFromActorInfo()->GetSet<UWeaponAttributeSet>();
	bool bFound = false;
	float GrenadeCurrentCharges = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(
		UWeaponAttributeSet::GetPlasmaGrenadeCurrentChargesAttribute(), bFound);
	
	// Clear equip slot and equip the throwable again to simulate grabbing another grenade from the inventory
	if (WeaponAS && GrenadeCurrentCharges > 0.f)
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
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if (SpawnedGrenadePreviewActor) SpawnedGrenadePreviewActor->Destroy();
}
