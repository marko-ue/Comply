// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Utility_Ranger.h"

#include "Abilities/Tasks/AbilityTask_WaitDelay.h"


void UUtility_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Use();
	
	UAbilityTask_WaitDelay* WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, ShieldLifetime);
	WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnShieldExpired);
	WaitDelayTask->ReadyForActivation();
}

void UUtility_Ranger::Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Ranger utility activated"));
	
	TraceAndSpawnShield();
}

void UUtility_Ranger::OnShieldExpired()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UUtility_Ranger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SpawnedActor.IsValid())
	{
		SpawnedActor->Destroy();
		SpawnedActor = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
		SpawnedActor = GetWorld()->SpawnActor<AActor>(ShieldActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}
