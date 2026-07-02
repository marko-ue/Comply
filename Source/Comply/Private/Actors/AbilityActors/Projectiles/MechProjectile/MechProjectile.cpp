// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectile.h"

#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectileAreaEffect.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


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

void AMechProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	
	LaunchProjectile();
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
	bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		this, LaunchVelocity, StartLoc, TargetLoc, 0.f, 0.35f
	);

	if (bSuccess)
	{
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
			AreaEffectClass, SpawnTransform, OwnerActor, InstigatorPawn, ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		
		if (MechProjectileAreaEffect)
		{
			MechProjectileAreaEffect->SourceASC = SourceASC;
			MechProjectileAreaEffect->TargetActor = TargetActor;

			UGameplayStatics::FinishSpawningActor(MechProjectileAreaEffect, SpawnTransform);
		}
	}

	Destroy();
}

