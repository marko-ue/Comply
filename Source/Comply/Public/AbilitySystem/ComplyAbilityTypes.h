#pragma once

#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "ComplyAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FComplyGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:
	// Returns the actual struct used for serialization, subclasses must override this
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	// Custom serialization, subclasses must override this
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
	virtual FComplyGameplayEffectContext* Duplicate() const override
	{
		FComplyGameplayEffectContext* NewContext = new FComplyGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

public:
	UPROPERTY()
	bool bHitThroughShield = false;
	
	UPROPERTY()
	float ShieldDamageMultiplier = 1.f;
	
	// Used to send the shotgun's trace target data for cue effect purposes
	FGameplayAbilityTargetDataHandle ShotgunTracesTargetData;
};

template<>
struct TStructOpsTypeTraits<FComplyGameplayEffectContext> : TStructOpsTypeTraitsBase2<FComplyGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};

USTRUCT()
struct FComplyGameplayAbilityTargetData_SingleHit : public FGameplayAbilityTargetData_SingleTargetHit
{
	GENERATED_BODY()

	UPROPERTY()
	bool bPassedThroughShield = false;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FGameplayAbilityTargetData* Clone() const
	{
		FComplyGameplayAbilityTargetData_SingleHit* NewData =
			new FComplyGameplayAbilityTargetData_SingleHit();

		NewData->HitResult = HitResult;
		NewData->bPassedThroughShield = bPassedThroughShield;

		return NewData;
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Super::NetSerialize(Ar, Map, bOutSuccess);
		Ar << bPassedThroughShield;
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FComplyGameplayAbilityTargetData_SingleHit>
	: TStructOpsTypeTraitsBase2<FComplyGameplayAbilityTargetData_SingleHit>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};