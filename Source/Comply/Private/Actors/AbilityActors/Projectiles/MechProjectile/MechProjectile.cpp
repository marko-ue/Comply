// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectile.h"

#include "AbilitySystem/Data/Enemy/Abilities/Mech/MechProjectileData.h"
#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectileAreaEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


AMechProjectile::AMechProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetNotifyRigidBodyCollision(true);
	SetRootComponent(ProjectileMesh);
	
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComp");
	ProjectileMovementComp->bShouldBounce = false;
	ProjectileMovementComp->SetUpdatedComponent(ProjectileMesh);
}

void AMechProjectile::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, InitialVelocity);
	DOREPLIFETIME(ThisClass, ProjectileData);
}

void AMechProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(ProjectileData, TEXT("ProjectileData not passed into %s"), *GetName());
	
	// Movement shouldn't be replicated as clients now have a local projectile for smoothness
	SetReplicateMovement(false);
	
	ProjectileMesh->SetStaticMesh(ProjectileData->ProjectileMesh);
	ProjectileMesh->SetMaterial(0, ProjectileData->ProjectileMaterial);
	
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	
	LaunchProjectile();
}

void AMechProjectile::OnRep_InitialVelocity() const
{
	ProjectileMovementComp->Velocity = InitialVelocity;
}

void AMechProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMechProjectile::LaunchProjectile()
{
	if (!TargetActor) return;
	
	FVector StartLoc = GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();

	// Sets the velocity of the projectile with an arc with a destination to the target location (target actor's location)
	FVector LaunchVelocity;
	const bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this, LaunchVelocity, StartLoc, TargetLoc, 0.f, 0.35f
	);

	if (bSuccess)
	{
		InitialVelocity = LaunchVelocity; // Triggers OnRep on clients
		ProjectileMovementComp->Velocity = LaunchVelocity;
	}
}

void AMechProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							FVector NormalImpulse, const FHitResult& Hit)
{
	// Align the actor's Up (Z) axis with the floor normal
	FRotator FlatRotation = FRotationMatrix::MakeFromZ(Hit.ImpactNormal).Rotator();
	const FTransform SpawnTransform(FlatRotation, Hit.ImpactPoint);
	
	// Once the projectile hits the ground, spawn its area effect that slows and damages
	if (HasAuthority())
	{
		AMechProjectileAreaEffect* MechProjectileAreaEffect = GetWorld()->SpawnActorDeferred<AMechProjectileAreaEffect>(
			ProjectileData->AreaEffectActorClass, SpawnTransform, OwnerActor, InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		
		if (MechProjectileAreaEffect)
		{
			MechProjectileAreaEffect->SourceASC = SourceASC;
			MechProjectileAreaEffect->TargetActor = TargetActor;
			MechProjectileAreaEffect->ProjectileData = ProjectileData;

			UGameplayStatics::FinishSpawningActor(MechProjectileAreaEffect, SpawnTransform);
		}
	}

	Destroy();
}
