// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/ThrowableAbilityBase.h"
#include "Throwable_Ranger.generated.h"

class APlasmaGrenadePreview;
/**
 * 
 */
UCLASS()
class COMPLY_API UThrowable_Ranger : public UThrowableAbilityBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	virtual void ConfirmThrow() override;
	
	UPROPERTY()
	TObjectPtr<APlasmaGrenadePreview> SpawnedGrenadePreview;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	void SpawnPreview();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> GrenadeActorClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> GrenadePreviewActorClass;
};
