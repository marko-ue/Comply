// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
};

UCLASS()
class COMPLY_API AComplyCharacterBase : public AComplyCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AComplyCharacterBase();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Applies the gameplay effect that initializes attribute values 
	// The function will be implemented here and called on child classes after we know the ASC is valid (server only)
	void InitializeAttributes() const;
	
	// Gives each ability in the StartupAbilities TArray to the character's ASC, and sets its input tag (server only)
	void GiveStartupAbilities();
	
	

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FAbilitySet> StartupAbilities;
};
