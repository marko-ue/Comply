// Copyright © 2026 Marko. All rights reserved.

#include "AbilitySystem/Abilities/Player/Ranger/Utility_Ranger.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/ComplyUtilityData.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/ShieldUtilityData.h"
#include "Actors/AbilityActors/ShieldDome/ShieldDome.h"
#include "Kismet/GameplayStatics.h"


void UUtility_Ranger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	checkf(UtilityData, TEXT("UtilityData not set on %s"), *GetName());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UUtility_Ranger::ConfirmPlacement()
{
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo()))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	Super::ConfirmPlacement();
}

void UUtility_Ranger::TraceAndSpawn()
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
			UtilityData->UtilityActorClass, 
			FTransform(FRotator::ZeroRotator, SpawnLocation),
			Avatar,
			Cast<APawn>(Avatar),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
    
		if (Shield)
		{
			Shield->SourceASC = GetAbilitySystemComponentFromActorInfo();
			Shield->ShieldData = Cast<UShieldUtilityData>(UtilityData);
			UGameplayStatics::FinishSpawningActor(Shield, FTransform(FRotator::ZeroRotator, SpawnLocation));
			Shield->SetLifeSpan(UtilityData->UtilityLifetime);
		}
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = SpawnLocation;
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::ShieldPlaced, CueParams);
	
	// Automatically equip the primary ability once the shield is thrown, as the player should not be able to equip the shield while it's on cooldown
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(FGameplayTagContainer(ComplyTags::ComplyAbilities::AssetTags::Equip_Primary));
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
