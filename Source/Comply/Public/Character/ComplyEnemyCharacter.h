// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "ComplyEnemyCharacter.generated.h"

class UComplyAttributeSet;
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
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
};
