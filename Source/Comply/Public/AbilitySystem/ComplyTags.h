#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Utility);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Throwable);
		
		namespace AssetTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ApplyShieldedEffectAbility);
			
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip_Primary)
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equip_Throwable)
		}
		
		namespace InputTags
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Primary);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_OneShotUtility);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Reload);
		}
		
		namespace DamageTypes
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical);
		}
	}
	
	namespace States
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Aiming);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Firing);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Shielded)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Reloading);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Distracted)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Equipping)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_TotemBuffed)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_ThrowablePreparing)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_NoThrowables)
	}
	
	namespace SetByCaller
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceRifleReserveAmmo);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceMagnumReserveAmmo);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_ReduceShotgunReserveAmmo);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(SBC_TotemSpeedBuff);
	}
	
	namespace GameplayCues
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitscanWeaponImpact);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShotgunImpact)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitscanWeaponFire)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponDryFire)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponReload)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(WeaponReloadFinished)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShieldPlaced)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShieldHitscanWeaponImpact)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ShieldShotgunImpact)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(PullGrenadePin)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ThrowGrenade)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ExplodeGrenade)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GrapplingHookUse)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GrapplingHookImpact)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GrapplingHookHooking)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurretTyping)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurretImpact)
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BuffTotemApplyBuff)
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(BuffTotemBuffAura)
	}
}
