// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Enemies/Mech/Attack_Mech.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"
#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectile.h"
#include "Kismet/GameplayStatics.h"


UAttack_Mech::UAttack_Mech()
{
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = ComplyTags::Events::Event_AttackMech;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UAttack_Mech::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return; 
	checkf(ProjectileData, TEXT("ProjectileData not set on %s"), *GetName());
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (TriggerEventData && TriggerEventData->Target)
	{
		TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	}
	
	UAbilityTask_PlayMontageAndWait* AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage, 1.f, NAME_None, true);
	AttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
	AttackMontageTask->ReadyForActivation();
	
	const FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FRotator SpawnRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
	
	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	
	APawn* InstigatorPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	AMechProjectile* MechProjectile = GetWorld()->SpawnActorDeferred<AMechProjectile>(
		MechProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(), InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	if (MechProjectile)
	{
		MechProjectile->TargetActor = TargetActor;
		MechProjectile->SourceASC = GetAbilitySystemComponentFromActorInfo();
		MechProjectile->OwnerActor = GetOwningActorFromActorInfo();
		MechProjectile->InstigatorPawn = InstigatorPawn;
		MechProjectile->ProjectileData = ProjectileData;

		UGameplayStatics::FinishSpawningActor(MechProjectile, SpawnTransform);
	}
}

void UAttack_Mech::OnAttackAnimationFinished()
{
	Super::OnAttackAnimationFinished();
}

void UAttack_Mech::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
