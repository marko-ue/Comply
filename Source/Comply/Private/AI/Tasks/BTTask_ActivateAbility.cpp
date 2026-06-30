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
}

// Initializes the BB
void UBTTask_ActivateAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
	}
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

    FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityToActivate);
    if (!Spec) return EBTNodeResult::Failed;

	// The ability is activated through a gameplay event through the event trigger tag for the ability
	// This is done so the target actor of the enemy can be passed in, and is used in the ability to apply damage to the target actor
	FGameplayEventData EventData;
	EventData.Target = Target;
	
	int32 TriggeredCount = ASC->HandleGameplayEvent(AbilityEventTag, &EventData);
	bool bActivated = TriggeredCount > 0;
	
	// Ends the task if the ability wasn't triggered (if on cooldown), so the event can be broadcasted again
	if (!bActivated)
	{
		return EBTNodeResult::Failed;
	}

    // Cache for everything needed to track this ability instance
    CachedASC = ASC;
    ActiveSpecHandle = Spec->Handle;
    CachedOwnerComp = &OwnerComp;

    AbilityEndedDelegateHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTask_ActivateAbility::OnAbilityEnded);

    // The task is now latent, and should remain InProgress and instead ended only when the ability ends
    return EBTNodeResult::InProgress;
}

// Callback for when this task's gameplay ability ends
void UBTTask_ActivateAbility::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle != ActiveSpecHandle) return;
    if (!CachedOwnerComp.IsValid()) return;

    if (CachedASC.IsValid())
    {
        CachedASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
    }

	// Only finish the task now, when the ability ends.
	// This prevents the enemy from moving while the ability is not ended
    FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_ActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (CachedASC.IsValid())
    {
        CachedASC->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);

        // Cancel the ability on abort to ensure cleanup in edge cases
        CachedASC->CancelAbilityHandle(ActiveSpecHandle);
    }

    return EBTNodeResult::Aborted;
}
