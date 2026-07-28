// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "GameFramework/Actor.h"
#include "MechProjectileAreaEffect.generated.h"

class UGameplayEffect;
class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UAbilitySystemComponent;
class USoundCue;

UCLASS()
class COMPLY_API AMechProjectileAreaEffect : public AActor
{
	GENERATED_BODY()

public:
	AMechProjectileAreaEffect();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY() 
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> AreaEffectMesh;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	FActiveGameplayEffectHandle ActiveDamageEffectHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> SlowEffectClass;
	
	FActiveGameplayEffectHandle ActiveSlowEffectHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Area Effect Settings")
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Area Effect Settings")
	FScalableFloat ExplicitDamage = 10.f;
	
	UPROPERTY(EditAnywhere, Category = "Area Effect Settings")
	float AreaEffectRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundCue> AreaEffectSound;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> AreaEffectAudioComponent;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraSystem> AreaEffectParticles;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> AreaEffectNiagaraComponent;
	
	// Tracks already affected actors to prevent multiple function calls on the same actor
	UPROPERTY()
	TSet<TObjectPtr<AActor>> AffectedActors;
	
	void ApplyEffectToTarget(AActor* OverlappingActor, UAbilitySystemComponent* TargetASC);
	void ApplySlowToTarget(UAbilitySystemComponent* TargetASC);
	void ApplyDamageToTarget(UAbilitySystemComponent* TargetASC);
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
