// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"
#include "GameFramework/Actor.h"
#include "MechProjectileAreaEffect.generated.h"

class UMechProjectileData;
class UGameplayEffect;
class USphereComponent;
class UNiagaraComponent;
class UAbilitySystemComponent;

UCLASS()
class COMPLY_API AMechProjectileAreaEffect : public AActor
{
	GENERATED_BODY()

public:
	AMechProjectileAreaEffect();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(Replicated)
	TObjectPtr<UMechProjectileData> ProjectileData;
	
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
	
	FActiveGameplayEffectHandle ActiveDamageEffectHandle;
	
	FActiveGameplayEffectHandle ActiveSlowEffectHandle;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> AreaEffectAudioComponent;
	
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
