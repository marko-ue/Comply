// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DeployableTurret.generated.h"

class UArrowComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
class USphereComponent;

UCLASS()
class COMPLY_API ADeployableTurret : public AActor
{
	GENERATED_BODY()

public:
	ADeployableTurret();
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY()
	float Damage = 100.f;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY()
	FGameplayTag DamageTypeTag;

protected:
	virtual void BeginPlay() override;
	
private:
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY()
	TArray<AActor*> TargetsInRange;
	
	FTimerHandle FireTimerHandle;
	
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	UFUNCTION()
	void TryFire();
	
	void Fire(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* TurretMesh;
	
	UPROPERTY(EditAnywhere)
	UArrowComponent* ArrowComp;
	
	UPROPERTY(EditAnywhere)
	USphereComponent* SphereComp;
};
