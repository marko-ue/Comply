// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldDome.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UAbilitySystemComponent;
class UGameplayAbility;
class USphereComponent;
class UNavModifierComponent;
class USoundCue;

UCLASS()
class COMPLY_API AShieldDome : public AActor
{
	GENERATED_BODY()

public:
	AShieldDome();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldStaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldObjectStaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayAbility> ApplyShieldedEffectAbilityClass;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ShieldParticleEffect;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ShieldHumNiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ShieldHummingCue;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> HumAudioComponent;
};
