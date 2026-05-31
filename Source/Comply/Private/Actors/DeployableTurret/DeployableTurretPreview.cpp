// Copyright © 2026 Marko. All rights reserved.


#include "Actors/DeployableTurret/DeployableTurretPreview.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


ADeployableTurretPreview::ADeployableTurretPreview()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(RootComponent);
}

void ADeployableTurretPreview::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADeployableTurretPreview::InitPreviewData(ACharacter* OwnerChar)
{
	OwnerCharacter = OwnerChar;
}

void ADeployableTurretPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdatePosition();
}

void ADeployableTurretPreview::UpdatePosition()
{
	if (!OwnerCharacter) return;
	if (!bShouldUpdatePosition) return;
	
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
		NewRotation.Yaw += 0.f;
		NewRotation.Pitch = 0.f;
		NewRotation.Roll = 0.f;
		SetActorRotation(NewRotation);

		FVector NewLocation = Start;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
		{
			// Check the slope of where the turret is attempted to get placed
			// Prevents placing the turret on steep slopes or in midair
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
		
		// Used in the ability so the turret is only spawned at the initial placement location
		// bShouldUpdatePosition is set to false when the input is initially confirmed and the turret was placed in a valid position
		PlacementLocation = GetActorLocation();
		PlacementRotation = GetActorRotation();
	}
}
