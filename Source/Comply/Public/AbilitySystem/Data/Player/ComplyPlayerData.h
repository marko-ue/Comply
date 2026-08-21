// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/ComplyCharacterStatData.h"
#include "Engine/DataAsset.h"
#include "ComplyPlayerData.generated.h"


// If override width is disabled, the width option can't be changed and width will not be overridden in the widget's size box
USTRUCT(BlueprintType)
struct FComplyWidgetSizeOverride
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	bool bOverrideWidth = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bOverrideWidth"))
	float Width = 200.f;
};

USTRUCT(BlueprintType)
struct FComplyHUDLayout
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FComplyWidgetSizeOverride AmmoWidget;

	UPROPERTY(EditDefaultsOnly)
	FComplyWidgetSizeOverride CooldownWidget;

	UPROPERTY(EditDefaultsOnly)
	FComplyWidgetSizeOverride ChargeWidget;
};

class UGameplayEffect;
/**
 * 
 */
UCLASS()
class COMPLY_API UComplyPlayerData : public UComplyCharacterStatData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeWeaponAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxArmor = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MovementSpeed = 500.f;
	
	// Zoom
	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	float DefaultFOV = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Zoom")
	float ZoomSpeed = 10.f;
	// End Zoom
	
	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	float RecoilReturnSpeed = 12.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Buffs")
	int32 TotemSpeedBonusPerStack = 48;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> PrepareReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TObjectPtr<UAnimMontage> ReviveMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	FComplyHUDLayout HUDLayout;
};
