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
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayAbility> ApplyShieldedEffectAbilityClass;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldStaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldObjectStaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ShieldParticles;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ShieldHumNiagaraComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> ShieldHummingSound;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> HumAudioComponent;
};
