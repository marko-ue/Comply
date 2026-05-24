// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Throwable_Ranger.h"

#include "Abilities/Tasks/AbilityTask_WaitConfirm.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/AttributeSets/WeaponAttributeSet.h"
#include "Actors/PlasmaGrenade/PlasmaGrenade.h"
#include "Actors/PlasmaGrenade/PlasmaGrenadePreview.h"
#include "GameFramework/ProjectileMovementComponent.h"
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

void UThrowable_Ranger::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if (SpawnedGrenadePreviewActor) SpawnedGrenadePreviewActor->Destroy();
}

// This function is overridden so ability costs can be handled manually
// The charge would usually get consumed when the input is pressed, doing it manually allows the player to use all charges
bool UThrowable_Ranger::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FGameplayTagContainer* OptionalRelevantTags)
{
	return true;
}

void UThrowable_Ranger::ConfirmThrow()
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
		ASC->Server_ThrowGrenade(GetCurrentAbilitySpecHandle(), InstigatorPawn->GetActorLocation(), InstigatorPawn->GetActorRotation(), LaunchVelocity);
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
}
