// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BuffTotem.generated.h"

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

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	FActiveGameplayEffectHandle ActiveTotemBuffEffect;
	
	void OnTotemBuffTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	// Will be upgradeable
	int32 BuffCount = 0;
};
