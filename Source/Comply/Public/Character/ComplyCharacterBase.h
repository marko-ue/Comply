// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "ComplyCharacter.h"

#include "ComplyCharacterBase.generated.h"

class UGameplayEffect;

UCLASS()
class COMPLY_API AComplyCharacterBase : public AComplyCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AComplyCharacterBase();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Function that applies the gameplay effect that initializes attribute values 
	// The function will be implemented here and called on child classes after we know the ASC is valid (server only)
	void InitializeAttributes() const;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;

};
