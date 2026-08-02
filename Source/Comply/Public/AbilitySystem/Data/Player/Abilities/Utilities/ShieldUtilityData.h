// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComplyUtilityData.h"
#include "ShieldUtilityData.generated.h"

class UGameplayAbility;
class UNiagaraSystem;
class USoundCue;
/**
 * 
 */
UCLASS()
class COMPLY_API UShieldUtilityData : public UComplyUtilityData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ShieldParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> ShieldHummingSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TSubclassOf<UGameplayAbility> ApplyShieldedEffectAbilityClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Meshes")
	TObjectPtr<UStaticMesh> ShieldObjectMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInterface> ShieldObjectMaterial;
};
