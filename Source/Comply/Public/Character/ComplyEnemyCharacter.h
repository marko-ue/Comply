// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "ComplyEnemyCharacter.generated.h"

class UWidgetComponent;
class UComplyEnemyData;
class UComplyEnemyAbilityData;
class UComplyAttributeSet;
class USoundCue;
/**
 * 
 */
UCLASS()
class COMPLY_API AComplyEnemyCharacter : public AComplyCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AComplyEnemyCharacter();
	
	virtual void OnRep_IsDead();
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyEnemyAbilityData> EnemyAbilityData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyEnemyData> EnemyData;
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundCue> AttackSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundCue> DeathSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundCue> FootstepsSound;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void InitializeAttributes() const override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> HealthWidgetComponent;
	
	void InitializeHealthWidgetComponent() const;
	void RotateHealthWidgetToPlayer();
};
