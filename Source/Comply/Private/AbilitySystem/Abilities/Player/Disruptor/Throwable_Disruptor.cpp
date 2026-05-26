// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Disruptor/Throwable_Disruptor.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "Actors/DecoyGrenade/DecoyGrenadePreview.h"
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
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
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
	
	if (SpawnedDecoyGrenadePreviewActor) SpawnedDecoyGrenadePreviewActor->Destroy();
}
