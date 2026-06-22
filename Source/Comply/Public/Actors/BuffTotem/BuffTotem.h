// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BuffTotem.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UGameplayEffect;
class UGameplayAbility;
class USphereComponent;
class USoundCue;

UCLASS()
class COMPLY_API ABuffTotem : public AActor
{
	GENERATED_BODY()

public:
	ABuffTotem();
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> ApplyTotemBuffEffectClass;

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Tracks the actors currently overlapping with the buff to prevent double applications in certain cases
	// Set used instead of an array due to the set being O(1)
	UPROPERTY()
	TSet<TObjectPtr<AActor>> OverlappingActors;
	
	FActiveGameplayEffectHandle ActiveTotemBuffEffect;

	// Will be upgradeable
	int32 BuffCount = 0;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> PlaceTotemImpactParticles;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> TotemParticles;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TotemNiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> TotemPlaceSound;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> TotemHummingSound;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> HumAudioComponent;
};
