// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "ComplyEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API AComplyEnemyCharacter : public AComplyCharacterBase
{
	GENERATED_BODY()

public:
	AComplyEnemyCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
};
