// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "ComplyPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMPLY_API AComplyPlayerCharacter : public AComplyCharacterBase
{
	GENERATED_BODY()
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	virtual void OnRep_PlayerState() override;
};
