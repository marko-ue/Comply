// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechProjectile.generated.h"

class UMechProjectileData;
class UAbilitySystemComponent;
class AMechProjectileAreaEffect;
class UProjectileMovementComponent;

UCLASS()
class COMPLY_API AMechProjectile : public AActor
{
	GENERATED_BODY()

public:
	AMechProjectile();
	
	void LaunchProjectile();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(Replicated)
	TObjectPtr<UMechProjectileData> ProjectileData;
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
	
	UPROPERTY() 
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	
	UPROPERTY()
	TObjectPtr<APawn> InstigatorPawn;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(ReplicatedUsing=OnRep_InitialVelocity)
	FVector InitialVelocity;
	
	UFUNCTION()
	void OnRep_InitialVelocity() const;
};
