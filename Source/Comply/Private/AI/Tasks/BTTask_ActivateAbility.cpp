// Copyright © 2026 Marko. All rights reserved.

#include "AI/Tasks/BTTask_ActivateAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "AbilitySystem/ComplyTags.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ActivateAbility::UBTTask_ActivateAbility()
{
    NodeName = "Activate Ability";
    bNotifyTaskFinished = true;
}

void UBTTask_ActivateAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);

    if (UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        TargetActorKey.ResolveSelectedKey(*BBAsset);
    }
}

// Tells the BT how many bytes to allocate in NodeMemory for each AI instance running this task
uint16 UBTTask_ActivateAbility::GetInstanceMemorySize() const
{
    return sizeof(FBTTask_ActivateAbilityMemory);
}

EBTNodeResult::Type UBTTask_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!Target) return EBTNodeResult::Failed;

    IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
    if (!ASCInterface) return EBTNodeResult::Failed;

    UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
    if (!ASC) return EBTNodeResult::Failed;

    if (!ASC->FindAbilitySpecFromClass(AbilityToActivate)) return EBTNodeResult::Failed;

    // The ability is activated through a gameplay event through the event trigger tag for the ability
    // This is done so the target actor of the enemy can be passed in, and is used in the ability to apply damage to the target actor
    FGameplayEventData EventData;
    EventData.Target = Target;

    int32 TriggeredCount = ASC->HandleGameplayEvent(AbilityEventTag, &EventData);

    // Ability wasn't triggered (like if it's on cooldown), fail so the BT can retry
    if (TriggeredCount <= 0) return EBTNodeResult::Failed;
    
    FGameplayAbilitySpec* SpecAfter = ASC->FindAbilitySpecFromClass(AbilityToActivate);
    if (!SpecAfter) return EBTNodeResult::Failed;

    // For InstancedPerExecution the new instance is pushed to NonReplicatedInstances during activation
    if (SpecAfter->NonReplicatedInstances.Num() == 0) return EBTNodeResult::Failed;

    // NodeMemory is a raw uint8*, casting to the struct so its fields can be read and written
    FBTTask_ActivateAbilityMemory* Memory = reinterpret_cast<FBTTask_ActivateAbilityMemory*>(NodeMemory);

    Memory->CachedAbilityInstance = SpecAfter->NonReplicatedInstances.Last().Get();
    Memory->CachedASC = ASC;
    Memory->ActiveSpecHandle = SpecAfter->Handle;
    Memory->CachedOwnerComp = &OwnerComp;

    // Capture Memory pointer in the lambda for the OnAbilityEnded binding
    // A member function callback for the same function can't receive NodeMemory, because the delegate is fixed to FAbilityEndedData
    Memory->AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddLambda(
        [this, Memory](const FAbilityEndedData& AbilityEndedData)
        {
            if (!Memory->CachedAbilityInstance.IsValid()) return;
            if (AbilityEndedData.AbilityThatEnded != Memory->CachedAbilityInstance.Get()) return;
            if (!Memory->CachedOwnerComp.IsValid()) return;

            if (Memory->CachedASC.IsValid())
            {
                Memory->CachedASC->OnAbilityEnded.Remove(Memory->AbilityEndedDelegateHandle);
            }

            FinishLatentTask(*Memory->CachedOwnerComp, EBTNodeResult::Succeeded);
        });

    return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_ActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FBTTask_ActivateAbilityMemory* Memory = reinterpret_cast<FBTTask_ActivateAbilityMemory*>(NodeMemory);

    if (Memory->CachedASC.IsValid())
    {
        Memory->CachedASC->OnAbilityEnded.Remove(Memory->AbilityEndedDelegateHandle);
        Memory->CachedASC->CancelAbilityHandle(Memory->ActiveSpecHandle);
    }

    return EBTNodeResult::Aborted;
}