// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ComplyPlayerState.generated.h"

class AComplyPlayerCharacter;
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

	// Tracks the character that was last selected before the mission was started
	// Set in the RPC when selecting characters, and read and set in BeginPlay/OnRep_PlayerState on the PC
	UPROPERTY()
	TSubclassOf<AComplyPlayerCharacter> LastSelectedCharacterClass;
	
	virtual void SetPlayerName(const FString& S) override;
	
	static FString TrimRadminSuffix(const FString& RawName);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY()
	TObjectPtr<UComplyAttributeSet> AttributeSet;
	
	UPROPERTY()
	TObjectPtr<UWeaponAttributeSet> WeaponAttributeSet;
};
