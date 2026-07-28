// Copyright © 2026 Marko. All rights reserved.

#include "AbilitySystem/Abilities/Enemies/Tank/ChargeAttack_Tank.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"
#include "Character/ComplyEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UChargeAttack_Tank::UChargeAttack_Tank()
{
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = ComplyTags::Events::Event_ChargeAttackTank;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UChargeAttack_Tank::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

    if (TriggerEventData && TriggerEventData->Target)
    {
        TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
    }

    if (!TargetActor) return;

    AComplyEnemyCharacter* Enemy = Cast<AComplyEnemyCharacter>(GetAvatarActorFromActorInfo());
    if (!Enemy) return;

    UCharacterMovementComponent* CMC = Enemy->GetCharacterMovement();
    if (!CMC) return;

    // Cache default values to restore later
    DefaultMaxFlySpeed = CMC->MaxFlySpeed;
    DefaultBrakingDeceleration = CMC->BrakingDecelerationFlying;

    UAbilityTask_PlayMontageAndWait* AttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, AttackMontage, 1.f, NAME_None, true
    );
    AttackMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
    AttackMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
    AttackMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnAttackAnimationFinished);
    AttackMontageTask->ReadyForActivation();

    // The launch velocity of the enemy will be towards the player scaled by charge speed
    FVector EnemyLocation = Enemy->GetActorLocation();
    FVector TargetLocation = TargetActor->GetActorLocation();
    FVector DirectionToTarget = (TargetLocation - EnemyLocation);
    DirectionToTarget.Z = 0.f;
    DirectionToTarget = DirectionToTarget.GetSafeNormal();
    FVector LaunchVelocity = DirectionToTarget * ChargeSpeed;

    // Ensure the enemy faces the target before charging
    Enemy->SetActorRotation(DirectionToTarget.Rotation());

    // Pause AI so the navmesh doesn't override the CMC velocity that's about to be set
    APawn* AIPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
    
    if (AAIController* AIC = AIPawn ? Cast<AAIController>(AIPawn->GetController()) : nullptr)
    {
        AIC->StopMovement();
        AIC->GetBrainComponent()->PauseLogic("Charging");
    }

    // Necessary settings to ensure the enemy is properly launched
    CMC->StopMovementImmediately();
    CMC->MaxFlySpeed = ChargeSpeed;
    CMC->BrakingDecelerationFlying = 0.f;
    CMC->Velocity = LaunchVelocity;
    CMC->SetMovementMode(MOVE_Flying);
}

void UChargeAttack_Tank::CancelAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateCancelAbility)
{
    RestoreMovement(ActorInfo);
    
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

// When the charge ends, all movement settings and the AI can be restored
void UChargeAttack_Tank::OnAttackAnimationFinished()
{
    RestoreMovement(GetCurrentActorInfo());

    if (AAIController* AIC = Cast<AAIController>(GetCurrentActorInfo()->AvatarActor->GetInstigatorController()))
    {
        AIC->GetBrainComponent()->ResumeLogic("Charging");
    }

    Super::OnAttackAnimationFinished();
}

// Restores all CMC settings to defaults. Called when the ability ends or gets canceled
void UChargeAttack_Tank::RestoreMovement(const FGameplayAbilityActorInfo* ActorInfo) const
{
    if (!ActorInfo) return;

    AComplyEnemyCharacter* Enemy = Cast<AComplyEnemyCharacter>(ActorInfo->AvatarActor.Get());
    if (!Enemy) return;

    UCharacterMovementComponent* CMC = Enemy->GetCharacterMovement();
    if (!CMC) return;

    CMC->StopMovementImmediately();
    CMC->MaxFlySpeed = DefaultMaxFlySpeed;
    CMC->BrakingDecelerationFlying = DefaultBrakingDeceleration;
    CMC->SetMovementMode(MOVE_Walking);
}
