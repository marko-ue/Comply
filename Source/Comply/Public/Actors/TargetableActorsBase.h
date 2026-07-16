// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Interface/TargetableInterface.h"
#include "TargetableActorsBase.generated.h"

class UGameplayEffect;
class UComplyAttributeSet;

UCLASS()
class COMPLY_API ATargetableActorsBase : public AActor, public ITargetableInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATargetableActorsBase();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void TakeDamage_Implementation() override;
	virtual void Die_Implementation() override;
	
	FORCEINLINE virtual UAbilitySystemComponent* GetTargetASC() const override { return GetAbilitySystemComponent(); }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
private:
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
};
