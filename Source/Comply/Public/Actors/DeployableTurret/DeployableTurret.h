// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interface/TargetableInterface.h"
#include "AbilitySystemInterface.h"
#include "DeployableTurret.generated.h"

class UComplyAttributeSet;
class UArrowComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
class USphereComponent;

UCLASS()
class COMPLY_API ADeployableTurret : public AActor, public ITargetableInterface, public IAbilitySystemInterface
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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY()
	FGameplayTag DamageTypeTag;
	
	FORCEINLINE virtual UAbilitySystemComponent* GetTargetASC() const override { return GetAbilitySystemComponent(); }
	
	virtual void Destroyed() override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> RechargeTurretChargeClass;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
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
