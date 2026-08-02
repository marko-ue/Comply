// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "ElectricHazardZone.generated.h"

class UElectricHazardZoneData;
class UAbilitySystemComponent;
class UGameplayEffect;
class UBoxComponent;
class UNiagaraComponent;
class UAudioComponent;

UCLASS()
class COMPLY_API AElectricHazardZone : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UElectricHazardZoneData> ElectricHazardZoneData;
	
	AElectricHazardZone();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> BoxComp;
	
	UPROPERTY(EditAnywhere, Category = "Effects")
	TObjectPtr<UNiagaraComponent> ElectricEffect;

	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<UAudioComponent> ElectricSound;
	
	FActiveGameplayEffectHandle ActiveDamageEffectHandle;
	
	FActiveGameplayEffectHandle ActiveStunEffectHandle;
	
	FTimerHandle ApplyStunEffectTimerHandle;
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void ApplyEffectToTarget(UAbilitySystemComponent* TargetASC);
	void ApplyStunToTarget(UAbilitySystemComponent* TargetASC);
	void ApplyDamageToTarget(UAbilitySystemComponent* TargetASC);
	
	// Tracks already affected actors to prevent multiple function calls on the same actor
	UPROPERTY()
	TSet<TObjectPtr<AActor>> AffectedActors;
};
