// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConfusionBeacon.generated.h"

class UGameplayEffect;
class UGameplayAbility;
class USphereComponent;

UCLASS()
class COMPLY_API AConfusionBeacon : public AActor
{
	GENERATED_BODY()

public:
	AConfusionBeacon();
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> SphereComp;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> ApplyConfusedEffectAbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ApplyConfusedEffectClass;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
