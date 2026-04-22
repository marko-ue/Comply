// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Player/PlayerInterface.h"
#include "ComplyPlayerCharacter.generated.h"


struct FActiveGameplayEffectHandle;
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> AimingEffectClass;
	
	FActiveGameplayEffectHandle ActiveAimingEffectHandle = FActiveGameplayEffectHandle();
	

	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	/* 
	 * Input
	*/
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PrimaryAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SecondaryAction;
	
	// Called for primary abilities
	void PrimaryActionPressed();
	
	// Called for secondary weapon actions
	void SecondaryActionPressed();
	void SecondaryActionReleased();
};
