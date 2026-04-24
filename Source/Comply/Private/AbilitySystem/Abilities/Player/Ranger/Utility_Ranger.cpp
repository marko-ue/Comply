// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/Ranger/Utility_Ranger.h"


void UUtility_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;
	
	CachedHandle = Handle;
	CachedActorInfo = ActorInfo;
	CachedActivationInfo = ActivationInfo;
	
	Use();
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void UUtility_Ranger::Use()
{
	UE_LOG(LogTemp, Warning, TEXT("Ranger utility activated"));
	
	AActor* Avatar = CachedActorInfo->AvatarActor.Get();
	if (!Avatar) return;

	FVector Forward = Avatar->GetActorForwardVector();
	FVector Start = Avatar->GetActorLocation() + Forward * 150.f;

	// trace to ground
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

	if (HasAuthority(&CachedActivationInfo))
	{
		GetWorld()->SpawnActor<AActor>(ShieldActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
}
