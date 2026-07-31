// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/BuffTotem/BuffTotemPreview.h"

#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


ABuffTotemPreview::ABuffTotemPreview()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABuffTotemPreview::BeginPlay()
{
	Super::BeginPlay();
}

void ABuffTotemPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UpdatePosition();
}

void ABuffTotemPreview::UpdatePosition()
{
	if (!OwnerCharacter) return;
	
	FVector TraceStart, TraceEnd, TraceDirection;
	if (!UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(this, OwnerCharacter, 500.f, TraceStart, TraceEnd, TraceDirection)) return;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(this);
	
	FRotator NewRotation = TraceDirection.Rotation();
	NewRotation.Yaw -= 90.f;
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
