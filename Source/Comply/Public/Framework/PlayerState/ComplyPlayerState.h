// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ComplyPlayerState.generated.h"

class UWeaponAttributeSet;
class UComplyAttributeSet;
/**
 * 
 */
UCLASS()
class COMPLY_API AComplyPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AComplyPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
	UPROPERTY()
	TObjectPtr<UWeaponAttributeSet> WeaponAttributeSet;
};
