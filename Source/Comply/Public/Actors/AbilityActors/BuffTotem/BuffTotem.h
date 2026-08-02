// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "BuffTotem.generated.h"

class UBuffTotemUtilityData;
class UNiagaraComponent;
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
	
	UPROPERTY(Replicated)
	TObjectPtr<UBuffTotemUtilityData> BuffTotemData;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
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
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TotemNiagaraComponent;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> HumAudioComponent;
};
