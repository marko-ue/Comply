// Copyright © 2026 Marko. All rights reserved.

#include "AI/Tasks/BTTask_ActivateAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ActivateAbility::UBTTask_ActivateAbility()
{
    NodeName = "Activate Ability";
    bNotifyTaskFinished = true;
    bCreateNodeInstance = false;
}

void UBTTask_ActivateAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
    Super::InitializeFromAsset(Asset);

    if (const UBlackboardData* BBAsset = GetBlackboardAsset())
    {
        TargetActorKey.ResolveSelectedKey(*BBAsset);
    }
}

uint16 UBTTask_ActivateAbility::GetInstanceMemorySize() const
{
    return sizeof(FBTTask_ActivateAbilityMemory);
}

EBTNodeResult::Type UBTTask_ActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
    if (!Target) return EBTNodeResult::Failed;

    const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
    if (!ASCInterface) return EBTNodeResult::Failed;

    UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
    if (!ASC) return EBTNodeResult::Failed;

    if (!ASC->FindAbilitySpecFromClass(AbilityToActivate)) return EBTNodeResult::Failed;

    // The ability is activated through a gameplay event so the target actor can be passed in
    FGameplayEventData EventData;
    EventData.Target = Target;

    const int32 TriggeredCount = ASC->HandleGameplayEvent(AbilityEventTag, &EventData);

    // Ability wasn't triggered (on cooldown), fail so the ability cooldown gates the retry
    if (TriggeredCount <= 0) return EBTNodeResult::Failed;

    return EBTNodeResult::Succeeded;
}

EBTNodeResult::Type UBTTask_ActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const FBTTask_ActivateAbilityMemory* Memory = reinterpret_cast<FBTTask_ActivateAbilityMemory*>(NodeMemory);

    if (Memory->CachedASC.IsValid())
    {
        Memory->CachedASC->CancelAbilityHandle(Memory->ActiveSpecHandle);
    }

    return EBTNodeResult::Aborted;
}