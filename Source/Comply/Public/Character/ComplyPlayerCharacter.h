// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Player/PlayerInterface.h"
#include "GameplayEffectTypes.h"
#include "Interface/TargetableInterface.h"
#include "ComplyPlayerCharacter.generated.h"


class URangedWeaponAbilityBase;
struct FActiveGameplayEffectHandle;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class COMPLY_API AComplyPlayerCharacter : public AComplyCharacterBase, public IPlayerInterface, public ITargetableInterface
{
	GENERATED_BODY()
	
public:
	AComplyPlayerCharacter();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	FORCEINLINE virtual UAbilitySystemComponent* GetTargetASC() const override { return GetAbilitySystemComponent(); }
	
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
	
	UPROPERTY(Replicated)
	TSubclassOf<URangedWeaponAbilityBase> EquippedPrimaryWeaponClass;
	
	URangedWeaponAbilityBase* GetEquippedPrimaryWeapon() const;
	void SetEquippedPrimaryWeapon(TSubclassOf<URangedWeaponAbilityBase> NewWeaponClass);
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> EquipPrimaryAbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> EquipUtilityAbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> EquipThrowableAbilityClass;
	
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
	
	UPROPERTY(meta=(DeprecatedProperty, DeprecationMessage="PrimaryAction now used for activating utilities"), EditAnywhere, Category="Input")
	UInputAction* UseUtilityAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* CancelPreviewAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* EquipPrimaryAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* EquipUtilityAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* EquipThrowableAction;
	
	// Called for primary abilities
	void PrimaryActionPressed();
	void PrimaryActionReleased();
	
	// Called for secondary weapon actions
	void SecondaryActionPressed();
	void SecondaryActionReleased();
	
	// Called for utility
	[[deprecated("Utility is now activated with primary input")]]
	void UseUtilityActionPressed();
	
	// Called for reloading
	void ReloadActionPressed();
	
	// Called for cancelling previews of certain abilities (cancels abilities)
	void CancelPreviewActionPressed();
	
	// Called for selecting the primary weapon
	void EquipPrimaryActionPressed();
	
	// Called for selecting the utility
	void EquipUtilityActionPressed();
	
	// Called for selecting the throwable
	void EquipThrowableActionPressed();
	
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
	
	UFUNCTION()
	void OnDistractedTagChanged(const FGameplayTag Tag, int32 NewCount);
};
