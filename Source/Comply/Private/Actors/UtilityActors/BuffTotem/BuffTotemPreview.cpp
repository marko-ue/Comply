// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/BuffTotem/BuffTotemPreview.h"

#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/BuffTotemUtilityData.h"
#include "GameFramework/Character.h"


ABuffTotemPreview::ABuffTotemPreview()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BuffTotemMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	BuffTotemMesh->SetupAttachment(RootComponent);
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

void ABuffTotemPreview::InitPreviewData(ACharacter* OwnerChar, const UComplyUtilityData* InUtilityData)
{
	checkf(InUtilityData, TEXT("UtilityData not passed into InitPreviewData on %s"), *GetName());
	
	Super::InitPreviewData(OwnerChar, InUtilityData);
	
	if (const UBuffTotemUtilityData* BuffTotemData = Cast<UBuffTotemUtilityData>(InUtilityData))
	{
		BuffTotemMesh->SetStaticMesh(BuffTotemData->UtilityMesh);
		ValidMaterial = BuffTotemData->ValidMaterial;
		InvalidMaterial = BuffTotemData->InvalidMaterial;
	}
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
			BuffTotemMesh->SetMaterial(0, ValidMaterial);
		}
		else
		{
			BuffTotemMesh->SetMaterial(0, InvalidMaterial);
		}
	}
	else
	{
		bCanPlace = false;
		BuffTotemMesh->SetMaterial(0, InvalidMaterial);
	}
	
	SetActorLocation(NewLocation);
}
