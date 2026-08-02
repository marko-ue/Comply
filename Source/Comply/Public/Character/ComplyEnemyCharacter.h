// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "ComplyEnemyCharacter.generated.h"

class UComplyEnemyCharacterStatData;
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
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyEnemyAbilityData> EnemyAbilityData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyEnemyCharacterStatData> EnemyStatData;
	
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
	
	virtual void InitializeAttributes() const override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
};
