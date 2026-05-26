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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	FORCEINLINE virtual UAbilitySystemComponent* GetTargetASC() const override { return GetAbilitySystemComponent(); }
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> RechargeTurretChargeClass;
	
	UPROPERTY()
	float Damage = 100.f;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY()
	FGameplayTag DamageTypeTag;
	
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Components")
	USkeletalMeshComponent* TurretMesh;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	UArrowComponent* ArrowComp;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	USphereComponent* SphereComp;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void TryFire();
	
	void Fire(AActor* TargetActor) const;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
	UPROPERTY()
	TArray<AActor*> TargetsInRange;
	
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	FTimerHandle FireTimerHandle;
};
