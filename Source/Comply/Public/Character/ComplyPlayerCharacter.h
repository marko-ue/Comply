// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Character/ComplyCharacterBase.h"
#include "Interface/Player/PlayerInterface.h"
#include "GameplayEffectTypes.h"
#include "Interface/TargetableInterface.h"
#include "Interface/Player/WeaponInterface.h"
#include "ComplyPlayerCharacter.generated.h"


class URangedWeaponAbilityBase;
struct FActiveGameplayEffectHandle;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class COMPLY_API AComplyPlayerCharacter : public AComplyCharacterBase, public IPlayerInterface, public ITargetableInterface, public IWeaponInterface
{
	GENERATED_BODY()
	
public:
	AComplyPlayerCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	FORCEINLINE virtual UAbilitySystemComponent* GetTargetASC() const override { return GetAbilitySystemComponent(); }
	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(Replicated)
	TSubclassOf<URangedWeaponAbilityBase> EquippedPrimaryWeaponClass;
	
	void SetEquippedPrimaryWeapon(TSubclassOf<URangedWeaponAbilityBase> NewWeaponClass);
	URangedWeaponAbilityBase* GetEquippedPrimaryWeapon() const;
	
	// Zoom
	UPROPERTY(EditAnywhere, Category = "Zoom")
	float DefaultFOV = 90.f;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float AimFOV = 60.f;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	float ZoomSpeed = 10.f;
	// End Zoom
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities|Effect Application")
	TSubclassOf<UGameplayAbility> ApplyAimEffectAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities|Effect Application")
	TSubclassOf<UGameplayAbility> ApplyFireEffectAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities|Equip")
	TSubclassOf<UGameplayAbility> EquipPrimaryAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities|Equip")
	TSubclassOf<UGameplayAbility> EquipUtilityAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities|Equip")
	TSubclassOf<UGameplayAbility> EquipThrowableAbilityClass;
	
	UPROPERTY(VisibleAnywhere)
	bool bFireInputHeld = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFiring = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsReloading = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Meshes")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> PrimaryMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> UtilityMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> ThrowableMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	FVector PrimaryMeshScale = FVector(1.0f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	FVector UtilityMeshScale = FVector(1.0f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	FVector ThrowableMeshScale = FVector(1.0f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> PrimaryEquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> UtilityEquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ThrowableEquipMontage;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentEquippedSlot)
	EWeaponSlot CurrentEquippedSlot = EWeaponSlot::Primary;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	// Input
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* PrimaryAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* SecondaryAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* ReloadAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* CancelPreviewAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* EquipPrimaryAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* EquipUtilityAction;
	
	UPROPERTY(EditAnywhere, Category="Input|Actions")
	UInputAction* EquipThrowableAction;

	void PrimaryActionPressed();
	void PrimaryActionReleased();

	void SecondaryActionPressed();
	void SecondaryActionReleased();

	void ReloadActionPressed();
	
	// Called for cancelling previews of certain abilities (cancels abilities)
	void CancelPreviewActionPressed();

	void EquipPrimaryActionPressed();

	void EquipUtilityActionPressed();

	void EquipThrowableActionPressed();
	// End Input
	
private:
	// Zoom
	void ZoomIn(float DeltaTime);
	void ZoomOut(float DeltaTime);
	
	// Whenever the tag for the Aiming State changes, call this function which will set a boolean to true or false depending on NewCount (whether it exists)
	UFUNCTION()
	void OnAimingTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	bool bIsAiming = false;
	// End Zoom
	
	void UpdateRotationMode(float DeltaTime);
	
	UFUNCTION()
	void OnDistractedTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	UFUNCTION()
	void OnTotemBuffedTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	virtual void OnWeaponEquipped(EWeaponSlot Slot) override;
	
	UFUNCTION()
	void OnRep_CurrentEquippedSlot();
	
	virtual FVector GetScaleForSlot(EWeaponSlot Slot) override;
	
	UFUNCTION(BlueprintCallable)
	UStaticMesh* GetMeshForSlot(EWeaponSlot Slot) override;
	
	virtual void ClearEquippedWeapon() override;
	
	UFUNCTION(BlueprintCallable)
	void OnWeaponDrawn();
};
