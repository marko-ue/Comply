// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MechProjectile.generated.h"

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

	virtual void Tick(float DeltaTime) override;
	
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
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMechProjectileAreaEffect> AreaEffectClass;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
