// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "ComplyEnemyCharacter.generated.h"

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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundCue> AttackSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundCue> DeathSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundCue> FootstepsSound;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float BaseHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float BaseMaxHealth = 100.f;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float BaseArmorPenetration = 0.1f;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
	float BaseMaxArmorPenetration = 0.1f;
};
