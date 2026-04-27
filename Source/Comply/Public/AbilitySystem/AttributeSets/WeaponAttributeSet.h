// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "WeaponAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class COMPLY_API UWeaponAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RifleCurrentAmmo)
	FGameplayAttributeData RifleCurrentAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RifleMaxAmmo)
	FGameplayAttributeData RifleMaxAmmo;
	
	UFUNCTION()
	void OnRep_RifleCurrentAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_RifleMaxAmmo(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, RifleCurrentAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, RifleMaxAmmo)
};
