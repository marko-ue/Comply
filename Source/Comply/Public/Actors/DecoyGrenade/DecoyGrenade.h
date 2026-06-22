// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DecoyGrenade.generated.h"

class UNiagaraSystem;
class UBlackboardComponent;
class UProjectileMovementComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class USoundCue;

UCLASS()
class COMPLY_API ADecoyGrenade : public AActor
{
	GENERATED_BODY()

public:
	ADecoyGrenade();
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComp;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* GrenadeMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ApplyDistractedEffectClass;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY()
	FGameplayTag DamageTypeTag;
	
	UPROPERTY()
	float PullRadius = 1000.f;
	
	UPROPERTY()
	float DecoyGrenadeLifetime = 20.f;
	
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void Explode();
	
	UPROPERTY()
	float MinDamage = 10.f;
	
	UPROPERTY()
	TArray<UAbilitySystemComponent*> AffectedASCs;
	
	UPROPERTY()
	TArray<UBlackboardComponent*> AffectedBBs;
	
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> DistractedEffectHandles;
	
	FTimerHandle ExplosionTimerHandle;
	
	FTimerHandle DestroyDelayTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<USoundCue> ExplodeSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UNiagaraSystem> ExplodeParticles;
};
