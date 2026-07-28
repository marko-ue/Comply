// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/DeployableTurret/DeployableTurretPreview.h"

#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
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
	
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, OwnerCharacter, 500.f, TraceStart, TraceEnd, TraceDirection)) return;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(this);
	
	FRotator NewRotation = TraceDirection.Rotation();
	NewRotation.Yaw += 0.f;
	NewRotation.Pitch = 0.f;
	NewRotation.Roll = 0.f;
	SetActorRotation(NewRotation);

	FVector NewLocation = TraceStart;
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		// Check the slope of where the turret is attempted to get placed
		// Prevents placing the turret on steep slopes or in midair
		float SlopeDot = FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector);
		bCanPlace = SlopeDot >= 0.85f;

		if (bCanPlace)
		{
			NewLocation = Hit.ImpactPoint;
			// Stores the last placement hit which the ability spawning this actor will use for the target data callback
			// which handles enabling replication and spawning the turret at the impact point, to show where a turret is about to be placed
			LastPlacementHit = Hit;
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
