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
