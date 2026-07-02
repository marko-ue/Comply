// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToUntilClearShot.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UBTTask_MoveToUntilClearShot : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveToUntilClearShot();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MaxAttackRange = 500.f;
	
};
