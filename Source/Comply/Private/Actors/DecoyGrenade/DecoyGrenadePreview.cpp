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
	
	FVector LaunchVelocity = FVector::ZeroVector;
	FVector2D ViewportSize = FVector2D();
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(
		this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		LaunchVelocity = CrosshairWorldDirection * ThrowSpeed;
	}

	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = OwningPawn->GetActorLocation() + FVector(0.f, 0.f, 60.f);
	PredictParams.LaunchVelocity = LaunchVelocity;
	PredictParams.ActorsToIgnore = ActorsToIgnore;
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime = 4.f;
	PredictParams.SimFrequency = 15.f;

	FPredictProjectilePathResult PredictResult;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);

	if (OwningPawn->IsLocallyControlled())
	{
		// Draw debug spheres along the predicted projectile path for visualization
		for (const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
		{
			DrawDebugSphere(GetWorld(), PathPoint.Location, 8.f, 6, FColor::Green, false, -1.f);
		}
	}
}

