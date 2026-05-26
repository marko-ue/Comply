// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ComplyCharacter.h"
#include "GameplayTagContainer.h"
#include "ComplyCharacterBase.generated.h"


class UGameplayAbility;
class UGameplayEffect;

// Maps abilities to input tags, set when adding the struct to startup abilities in blueprint
USTRUCT()
struct FAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityTypeTag;
};

UCLASS()
class COMPLY_API AComplyCharacterBase : public AComplyCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AComplyCharacterBase();
	
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Applies the gameplay effect that initializes attribute values 
	// The function will be implemented here and called on child classes after we know the ASC is valid (server only)
	void InitializeAttributes() const;
	
	// Gives each ability in the StartupAbilities TArray to the character's ASC, and sets its input tag (server only)
	void GiveStartupAbilities();
	
	void ActivateInitialAbility() const;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeWeaponAttributesEffect;
	
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FAbilitySet> StartupAbilities;
};
