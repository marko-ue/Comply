// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "Interface/TargetableInterface.h"
#include "TargetableActorsBase.generated.h"

class UNiagaraSystem;
class UGameplayEffect;
class UComplyAttributeSet;
class USoundCue;

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
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(VisibleAnywhere, Category = "Actor Properties")
	bool bWasKilledByEnemies = false;
	
private:
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Audio")
	TObjectPtr<USoundCue> DamageSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Audio")
	TObjectPtr<USoundCue> DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Effects")
	TObjectPtr<UNiagaraSystem> DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Effects")
	TObjectPtr<UNiagaraSystem> DeathEffect;
};
