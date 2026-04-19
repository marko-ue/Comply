// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Player/PlayerInterface.h"
#include "ComplyPlayerCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;

UCLASS()
class COMPLY_API AComplyPlayerCharacter : public AComplyCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
	
public:
	AComplyPlayerCharacter();
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
};
