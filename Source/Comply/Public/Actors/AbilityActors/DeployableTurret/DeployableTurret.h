// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interface/TargetableInterface.h"
#include "AbilitySystemInterface.h"
#include "Actors/TargetableActorsBase.h"
#include "DeployableTurret.generated.h"

class UDeployableTurretAbilityData;
class UNiagaraComponent;
class UComplyAttributeSet;
class UArrowComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
class USphereComponent;

UCLASS()
class COMPLY_API ADeployableTurret : public ATargetableActorsBase
{
	GENERATED_BODY()

public:
	ADeployableTurret();
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY()
	TObjectPtr<UDeployableTurretAbilityData> TurretData;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TurretMesh;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowComp;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void TryFire();
	
	void Fire(AActor* TargetActor);
	
	UPROPERTY()
	TArray<AActor*> TargetsInRange;
	
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	FTimerHandle FireTimerHandle;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> PlaceTurretNiagaraComponent;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_TurretFire();
};
