// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Player/PlayerInterface.h"
#include "GameplayEffectTypes.h"
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
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	/*
	 * Zooming in/out
	 */
	
	UPROPERTY(EditAnywhere, Category = "Camera")
	float DefaultFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float AimFOV = 60.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomSpeed = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> ApplyAimEffectAbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> ApplyFireEffectAbilityClass;
	
	bool bFireInputHeld = false;
	
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
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* UseUtilityAction;
	
	// Called for primary abilities
	void PrimaryActionPressed();
	void PrimaryActionReleased();
	
	// Called for secondary weapon actions
	void SecondaryActionPressed();
	void SecondaryActionReleased();
	
	// Called for utility
	void UseUtilityActionPressed();
	
private:
	/*
	 * Aiming and zooming in/out
	 */
	// Whenever the tag for the Aiming State changes, call this function which will set a boolean to true or false depending on NewCount (whether it exists)
	UFUNCTION()
	void OnAimingTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	void ZoomIn(float DeltaTime);
	void ZoomOut(float DeltaTime);
	
	bool bIsAiming = false;
};
