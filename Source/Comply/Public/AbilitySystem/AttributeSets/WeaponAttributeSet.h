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
	
	/* Rifle */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RifleCurrentAmmo)
	FGameplayAttributeData RifleCurrentAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RifleMaxAmmo)
	FGameplayAttributeData RifleMaxAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RifleCurrentReserveAmmo)
	FGameplayAttributeData RifleCurrentReserveAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RifleMaxReserveAmmo)
	FGameplayAttributeData RifleMaxReserveAmmo;
	
	UFUNCTION()
	void OnRep_RifleCurrentAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_RifleMaxAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_RifleCurrentReserveAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_RifleMaxReserveAmmo(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, RifleCurrentAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, RifleMaxAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, RifleCurrentReserveAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, RifleMaxReserveAmmo)
	/* End Rifle */
	
	/* Plasma Grenade */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlasmaGrenadeCurrentCharges)
	FGameplayAttributeData PlasmaGrenadeCurrentCharges;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PlasmaGrenadeMaxCharges)
	FGameplayAttributeData PlasmaGrenadeMaxCharges;
	
	UFUNCTION()
    void OnRep_PlasmaGrenadeCurrentCharges(const FGameplayAttributeData& OldValue);
    	
    UFUNCTION()
    void OnRep_PlasmaGrenadeMaxCharges(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, PlasmaGrenadeCurrentCharges)
	ATTRIBUTE_ACCESSORS(ThisClass, PlasmaGrenadeMaxCharges)
	/* End Plasma Grenade */
	
	/* Magnum */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagnumCurrentAmmo)
	FGameplayAttributeData MagnumCurrentAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagnumMaxAmmo)
	FGameplayAttributeData MagnumMaxAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagnumCurrentReserveAmmo)
	FGameplayAttributeData MagnumCurrentReserveAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagnumMaxReserveAmmo)
	FGameplayAttributeData MagnumMaxReserveAmmo;
	
	UFUNCTION()
	void OnRep_MagnumCurrentAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MagnumMaxAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MagnumCurrentReserveAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MagnumMaxReserveAmmo(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, MagnumCurrentAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, MagnumMaxAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, MagnumCurrentReserveAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, MagnumMaxReserveAmmo)
	/* End Magnum */
	
	/* Turret */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TurretCurrentCharges)
	FGameplayAttributeData TurretCurrentCharges;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TurretMaxCharges)
	FGameplayAttributeData TurretMaxCharges;
	
	UFUNCTION()
	void OnRep_TurretCurrentCharges(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_TurretMaxCharges(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, TurretCurrentCharges)
	ATTRIBUTE_ACCESSORS(ThisClass, TurretMaxCharges)
	/* End Turret */
	
	/* Shotgun */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShotgunCurrentAmmo)
	FGameplayAttributeData ShotgunCurrentAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShotgunMaxAmmo)
	FGameplayAttributeData ShotgunMaxAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShotgunCurrentReserveAmmo)
	FGameplayAttributeData ShotgunCurrentReserveAmmo;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ShotgunMaxReserveAmmo)
	FGameplayAttributeData ShotgunMaxReserveAmmo;
	
	UFUNCTION()
	void OnRep_ShotgunCurrentAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_ShotgunMaxAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_ShotgunCurrentReserveAmmo(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_ShotgunMaxReserveAmmo(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, ShotgunCurrentAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, ShotgunMaxAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, ShotgunCurrentReserveAmmo)
	ATTRIBUTE_ACCESSORS(ThisClass, ShotgunMaxReserveAmmo)
	/* End Shotgun */
	
	/* Plasma Grenade */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DecoyGrenadeCurrentCharges)
	FGameplayAttributeData DecoyGrenadeCurrentCharges;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DecoyGrenadeMaxCharges)
	FGameplayAttributeData DecoyGrenadeMaxCharges;
	
	UFUNCTION()
	void OnRep_DecoyGrenadeCurrentCharges(const FGameplayAttributeData& OldValue);
    	
	UFUNCTION()
	void OnRep_DecoyGrenadeMaxCharges(const FGameplayAttributeData& OldValue);
	
	ATTRIBUTE_ACCESSORS(ThisClass, DecoyGrenadeCurrentCharges)
	ATTRIBUTE_ACCESSORS(ThisClass, DecoyGrenadeMaxCharges)
	/* End Plasma Grenade */
};
