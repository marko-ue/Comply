// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "DecoyGrenade.generated.h"

class UDecoyGrenadeData;
class URotatingMovementComponent;
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
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ApplyDistractedEffectClass;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	UPROPERTY(Replicated)
	TObjectPtr<UDecoyGrenadeData> GrenadeData;
	
	UPROPERTY(ReplicatedUsing=OnRep_LaunchVelocity)
	FVector LaunchVelocity;
	
	UFUNCTION()
	void OnRep_LaunchVelocity() const;
	
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComp;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	UStaticMeshComponent* GrenadeMesh;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<URotatingMovementComponent> RotatingMovementComp;
	
	UFUNCTION()
	void Explode();
	
	UPROPERTY()
	TArray<UAbilitySystemComponent*> AffectedASCs;
	
	UPROPERTY()
	TArray<UBlackboardComponent*> AffectedBBs;
	
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> DistractedEffectHandles;
	
	FTimerHandle ExplosionTimerHandle;
	
	FTimerHandle DestroyDelayTimerHandle;
	
	// Sounds and particles stored as member variables are used in a gameplay cue
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Audio")
	TObjectPtr<USoundCue> ExplodeSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Effects")
	TObjectPtr<UNiagaraSystem> ExplodeParticles;
	
	UFUNCTION()
	void OnGrenadeLanded(const FHitResult& ImpactResult);
};
