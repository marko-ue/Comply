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


class UComplyInputData;
class UComplyPlayerData;
class UNiagaraSystem;
class IInteractableInterface;
class UInputMappingContext;
class URangedWeaponAbilityBase;
struct FActiveGameplayEffectHandle;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;

UCLASS()
class COMPLY_API AComplyPlayerCharacter : public AComplyCharacterBase, public IPlayerInterface, public ITargetableInterface, public IWeaponInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyPlayerData> PlayerData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<UComplyInputData> InputData;
	
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
	
	// Death and reviving
	UFUNCTION(BlueprintCallable)
	void DownPlayer();
	void RevivePlayer();
	
	UFUNCTION(Server, Reliable)
	void Server_ReviveTarget(AComplyPlayerCharacter* Target);
	
	UPROPERTY(ReplicatedUsing=OnRep_IsDowned)
	bool bIsDowned = false;
	// End Death and Reviving
	
	UFUNCTION(Server, Reliable)
	void Server_FaceTarget(ACharacter* Target);
	
	UPROPERTY(VisibleAnywhere, Category = "Player Properties")
	float NextSlowMagnitude;
	
	// Impact Effects
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpawnImpactEffects(FVector ImpactPoint, FVector ImpactNormal, FVector MuzzleLocation);
	
	void SpawnImpactEffectsLocal(const FVector& ImpactPoint, const FVector& ImpactNormal, const FVector& MuzzleLocation);
	// End Impact Effects
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|Buffs")
	TSubclassOf<UGameplayEffect> TotemSpeedBuffEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|States")
	TSubclassOf<UGameplayEffect> DownedEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects|States")
	TSubclassOf<UGameplayEffect> ReviveEffectClass;
	
	FActiveGameplayEffectHandle ActiveDownedEffectHandle;
	
	UPROPERTY(VisibleAnywhere)
	bool bFireInputHeld = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFiring = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bFiredThisFrame = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsReloading = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Settings|Meshes")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentEquippedSlot)
	EWeaponSlot CurrentEquippedSlot = EWeaponSlot::Primary;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	
	virtual void InitializeAttributes() const override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void PrimaryActionPressed();
	void PrimaryActionReleased();

	void SecondaryActionPressed();
	void SecondaryActionReleased();
	
	void InteractActionPressed();
	void InteractActionReleased();
	
	void SprintActionPressed();
	void SprintActionReleased();

	void ReloadActionPressed();
	
	// Called for cancelling previews of certain abilities (cancels abilities)
	void CancelPreviewActionPressed();

	void EquipPrimaryActionPressed();

	void EquipUtilityActionPressed();

	void EquipThrowableActionPressed();
	// End Input
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	// Zoom
	void ZoomIn(float DeltaTime) const;
	void ZoomOut(float DeltaTime) const;
	
	// Whenever the tag for the Aiming State changes, call this function which will set a boolean to true or false depending on NewCount (whether it exists)
	UFUNCTION()
	void OnAimingTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	bool bIsAiming = false;
	// End Zoom
	
	UFUNCTION()
	void OnRep_IsDowned();
	
	void TraceForInteractable();
	
	IInteractableInterface* CurrentFocusedInteractable;
	
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
	
	FActiveGameplayEffectHandle ActiveTotemSpeedBuffEffectHandle;
	
	void OnMovementSpeedAttributeChanged(const FOnAttributeChangeData& Data);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundCue> GunFireSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundCue> GunDryFireSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundCue> GunReloadSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = true))
	TObjectPtr<USoundCue> GunReloadFinishedSound;
};
