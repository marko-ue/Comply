// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ComplyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class COMPLY_API UComplyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	void HandleIncomingDamage(const struct FGameplayEffectModCallbackData& Data);
	
	/* 
	 * Primary Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, Health)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth)
	
	// TODO: Make upgradeable
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor)
	FGameplayAttributeData MaxArmor;
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);
	
	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldArmor);
	
	ATTRIBUTE_ACCESSORS(ThisClass, Armor)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxArmor)
	
	// Used for enemies, this will be used for scaling based on difficulty and amount of players
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration)
	FGameplayAttributeData ArmorPenetration;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmorPenetration)
	FGameplayAttributeData MaxArmorPenetration;
	
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration);
	
	UFUNCTION()
	void OnRep_MaxArmorPenetration(const FGameplayAttributeData& OldArmorPenetration);
	
	ATTRIBUTE_ACCESSORS(ThisClass, ArmorPenetration)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxArmorPenetration)
	
	/*
	 * Meta Attributes
	 */
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	
	ATTRIBUTE_ACCESSORS(ThisClass, IncomingDamage)
	
	/*
	 * Stats
	 */
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
	
	ATTRIBUTE_ACCESSORS(UComplyAttributeSet, MovementSpeed)
	
};
