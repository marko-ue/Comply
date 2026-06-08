#include "AbilitySystem/ComplyAbilityTypes.h"

bool FComplyGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	uint16 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid()) RepBits |= 1 << 0;
		if (bReplicateEffectCauser && EffectCauser.IsValid() ) RepBits |= 1 << 1;
		if (AbilityCDO.IsValid()) RepBits |= 1 << 2;
		if (bReplicateSourceObject && SourceObject.IsValid()) RepBits |= 1 << 3;
		if (Actors.Num() > 0) RepBits |= 1 << 4;
		if (HitResult.IsValid()) RepBits |= 1 << 5;
		if (bHasWorldOrigin) RepBits |= 1 << 6;
		if (bHitThroughShield) RepBits |= 1 << 7;
		if (ShieldDamageMultiplier != 1.f) RepBits |= 1 << 8;
		if (ShotgunTracesTargetData.IsValid(0)) RepBits |= 1 << 9;
	}
	
	Ar.SerializeBits(&RepBits, 10);
	
	if (RepBits & (1 << 0)) Ar << Instigator;
	if (RepBits & (1 << 1)) Ar << EffectCauser;
	if (RepBits & (1 << 2)) Ar << AbilityCDO;
	if (RepBits & (1 << 3)) Ar << SourceObject;
	if (RepBits & (1 << 4)) SafeNetSerializeTArray_Default<31>(Ar, Actors);
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else if (Ar.IsLoading())
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		Ar << bHitThroughShield;
	}
	else if (Ar.IsLoading())
	{
		bHitThroughShield = false;
	}
	if (RepBits & (1 << 8))
	{
		Ar << ShieldDamageMultiplier;
	}
	else if (Ar.IsLoading())
	{
		ShieldDamageMultiplier = 1.f;
	}
	if (RepBits & (1 << 9))
	{
		ShotgunTracesTargetData.NetSerialize(Ar, Map, bOutSuccess);
	}
	
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}	
	
	bOutSuccess = true;
	return true;
}
