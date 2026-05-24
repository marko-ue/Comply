// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DecoyGrenade.generated.h"

class UBlackboardComponent;
class UProjectileMovementComponent;
class UAbilitySystemComponent;
class UGameplayEffect;

UCLASS()
class COMPLY_API ADecoyGrenade : public AActor
{
	GENERATED_BODY()

public:
	ADecoyGrenade();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Destroyed() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ApplyDistractedEffectClass;
	
	UPROPERTY()
	float PullRadius = 1000.f;
	
	UPROPERTY()
	float DecoyGrenadeLifetime = 20.f;
	
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComp;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GrenadeMesh;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY()
	FGameplayTag DamageTypeTag;

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle ExplosionTimerHandle;
	
	FTimerHandle DestroyDelayTimerHandle;
	
	UFUNCTION()
	void Explode();
	
	UPROPERTY()
	float MinDamage = 10.f;
	
	UPROPERTY()
	TArray<UAbilitySystemComponent*> AffectedASCs;
	
	TArray<FActiveGameplayEffectHandle> DistractedEffectHandles;
	
	UPROPERTY()
	TArray<UBlackboardComponent*> AffectedBBs;
};
