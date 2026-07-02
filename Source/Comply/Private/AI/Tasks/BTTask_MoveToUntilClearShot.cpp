// Copyright © 2026 Marko. All rights reserved.


#include "AI/Tasks/BTTask_MoveToUntilClearShot.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MoveToUntilClearShot::UBTTask_MoveToUntilClearShot()
{
	NodeName = "Move To Until Clear Shot";
	bNotifyTick = true;
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveToUntilClearShot, TargetActorKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_MoveToUntilClearShot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Start moving and return InProgress - success is never returned based on distance
	AAIController* AIController = OwnerComp.GetAIOwner();
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
    
	if (!AIController || !Target) return EBTNodeResult::Failed;

	AIController->MoveToActor(Target, 0.f);
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToUntilClearShot::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController->GetPawn();
	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (!Pawn || !Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Check clear shot
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Pawn);
	Params.AddIgnoredActor(Target);

	const FCollisionShape SweepShape = FCollisionShape::MakeSphere(30.f); // Projectile size 

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit, Pawn->GetActorLocation(), Target->GetActorLocation(), FQuat::Identity, ECC_Visibility, SweepShape, Params
	);
	
	// Check distance to target, only succeed if there is a clear shot and the enemy is close enough to the target
	float DistanceToTarget = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

	if (!bHit && DistanceToTarget <= MaxAttackRange)
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// Still blocked or not far enough, keep updating MoveToActor each tick so it tracks a moving player
	AIController->MoveToActor(Target, 50.f);
}
