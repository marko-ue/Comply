// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FaceTarget.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API UBTService_FaceTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_FaceTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "Service Settings")
	float RotationInterpSpeed = 5.f;
};
