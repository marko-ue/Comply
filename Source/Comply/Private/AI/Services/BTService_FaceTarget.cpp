// Copyright © 2026 Marko. All rights reserved.


#include "AI/Services/BTService_FaceTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"


UBTService_FaceTarget::UBTService_FaceTarget()
{
	NodeName = "Face Target";
	
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FaceTarget, TargetActorKey), AActor::StaticClass());

	Interval = 0.1f;
	RandomDeviation = 0.1f;
}

void UBTService_FaceTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	APawn* ControlledPawn = AIController->GetPawn();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (!ControlledPawn || !Target) return;

	FVector Direction = Target->GetActorLocation() - ControlledPawn->GetActorLocation();
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0.f;

	FRotator CurrentRotation = ControlledPawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationInterpSpeed);

	ControlledPawn->SetActorRotation(NewRotation);
	AIController->SetControlRotation(NewRotation);
}
