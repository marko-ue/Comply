// Copyright © 2026 Marko. All rights reserved.


#include "AI/Tasks/BTTask_MoveToUntilClearShot.h"

#include "AIController.h"
#include "Comply.h"
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
	const APawn* Pawn = AIController->GetPawn();
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
	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_PlayerFriend);
	ObjectParams.AddObjectTypesToQuery(ECC_Player);

	const FCollisionShape SweepShape = FCollisionShape::MakeSphere(30.f); // Projectile size 

	const bool bHit = GetWorld()->SweepSingleByObjectType(
		Hit, Pawn->GetActorLocation(), Target->GetActorLocation(), FQuat::Identity, ObjectParams, SweepShape, Params
	);
	
	// Check distance to target, only succeed if there is a clear shot and the enemy is close enough to the target
	const float DistanceToTarget = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

	// Handles both player and targetable actor cases, by checking if nothing was hit or if the first thing hit was the target actor itself
	const bool bClearShot = !bHit || Hit.GetActor() == Target;

	if (bClearShot && DistanceToTarget <= MaxAttackRange)
	{
		AIController->StopMovement();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	// Still blocked or not far enough, keep updating MoveToActor each tick so it tracks a moving player
	AIController->MoveToActor(Target, 50.f);
}
