// Copyright © 2026 Marko. All rights reserved.


#include "Actors/DecoyGrenade/DecoyGrenadePreview.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"


ADecoyGrenadePreview::ADecoyGrenadePreview()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADecoyGrenadePreview::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADecoyGrenadePreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FVector LaunchVelocity = OwningPawn->GetActorForwardVector() * ThrowSpeed;

	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = OwningPawn->GetActorLocation();
	PredictParams.LaunchVelocity = LaunchVelocity;
	PredictParams.ActorsToIgnore = ActorsToIgnore;
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime = 4.f;
	PredictParams.SimFrequency = 15.f;

	FPredictProjectilePathResult PredictResult;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);

	// Draw debug spheres along the predicted projectile path for visualization
	for (const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
	{
		DrawDebugSphere(GetWorld(), PathPoint.Location, 8.f, 6, FColor::Green, false, -1.f);
	}
}

