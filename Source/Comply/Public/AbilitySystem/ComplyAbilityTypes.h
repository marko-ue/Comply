#pragma once

#include "GameplayEffectTypes.h"
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