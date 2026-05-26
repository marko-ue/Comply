// Copyright © 2026 Marko. All rights reserved.


#include "Actors/ConfusionBeacon/ConfusionBeaconPreview.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


AConfusionBeaconPreview::AConfusionBeaconPreview()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AConfusionBeaconPreview::BeginPlay()
{
	Super::BeginPlay();
}

void AConfusionBeaconPreview::InitPreviewData(ACharacter* OwnerChar)
{
	OwnerCharacter = OwnerChar;
}

void AConfusionBeaconPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdatePosition();
}

void AConfusionBeaconPreview::UpdatePosition()
{
	if (!OwnerCharacter) return;
	
	// Trace to the middle of the screen (crosshair)
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
		FVector Start = CrosshairWorldPosition;
		
		if (OwnerCharacter)
		{
			float DistanceToCharacter = (OwnerCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100);
		}
		
		FVector End = Start + CrosshairWorldDirection * 500;
		
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(OwnerCharacter);
		Params.AddIgnoredActor(this);
		
		FRotator NewRotation = CrosshairWorldDirection.Rotation();
		NewRotation.Yaw -= 90.f;
		NewRotation.Pitch = 0.f;
		NewRotation.Roll = 0.f;
		SetActorRotation(NewRotation);
		
		FVector NewLocation = Start;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
		{
			// Check the slope of where the turret is attempted to get placed
			// Prevents placing the turret on steep slopes or in mid-air
			float SlopeDot = FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector);
			bCanPlace = SlopeDot >= 0.85f;

			if (bCanPlace)
			{
				NewLocation = Hit.ImpactPoint;
				FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, ValidMaterial);
			}
			else
			{
				FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, InvalidMaterial);
			}
		}
		else
		{
			bCanPlace = false;
			FindComponentByClass<UStaticMeshComponent>()->SetMaterial(0, InvalidMaterial);
		}
		
		SetActorLocation(NewLocation);
	}
}
