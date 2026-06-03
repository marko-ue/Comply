#include "AbilitySystem/ComplyTags.h"


namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ComplyTags.Abilities.AbilityTags.Primary", "Primary ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Utility, "ComplyTags.Abilities.AbilityTags.Utility", "Utility ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Throwable, "ComplyTags.Abilities.AbilityTags.Throwable", "Throwable ability asset tag")
		
		namespace AssetTags
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ApplyShieldedEffectAbility, "ComplyTags.Abilities.Ranger.ApplyShieldedEffectAbility", "Apply shielded effect ability asset tag")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equip_Primary, "ComplyTags.Abilities.Equip.Primary", "Primary weapon equip ability asset tag")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equip_Throwable, "ComplyTags.Abilities.Equip.Throwable", "Throwable weapon equip ability asset tag")
		}
		
		namespace InputTags
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Primary, "ComplyTags.Abilities.InputTags.Input_Primary", "Input tag for primary abilities")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_OneShotUtility, "ComplyTags.Abilities.InputTags.Input_OneShotUtility", "Input tag for utilities that are used as soon as their input is pressed")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Reload, "ComplyTags.Abilities.InputTags.Input_Reload", "Input tag for reloading")
		}
		
		namespace DamageTypes
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "ComplyTags.Abilities.DamageTypes.Physical", "Physical damage type")
		}
	}
	
	namespace States
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Aiming, "ComplyTags.States.Aiming", "Is player aiming")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Firing, "ComplyTags.States.Firing", "Is player firing")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Shielded, "ComplyTags.States.Shielded", "Is player being shielded")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Reloading, "ComplyTags.States.Reloading", "Is player reloading")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Distracted, "ComplyTags.States.Distracted", "Is enemy distracted")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Equipping, "ComplyTags.States.Equipping", "Is player equipping")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_ThrowablePreparing, "ComplyTags.States.Throwable.Preparing", "Is player preparing throwable")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_NoThrowables, "ComplyTags.States.NoGrenades", "Does player have no throwables")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TotemBuffed, "ComplyTags.States.TotemBuffed", "Is player buffed by a totem")
	}
	
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SBC_ReduceRifleReserveAmmo, "ComplyTags.SetByCaller.Weapons.Rifle", "Set by caller tag for reducing rifle reserve ammo")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SBC_ReduceMagnumReserveAmmo, "ComplyTags.SetByCaller.Weapons.Magnum", "Set by caller tag for reducing magnum reserve ammo")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SBC_ReduceShotgunReserveAmmo, "ComplyTags.SetByCaller.Weapons.Shotgun", "Set by caller tag for reducing shotgun reserve ammo")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SBC_TotemSpeedBuff, "ComplyTags.SetByCaller.Buffs.TotemSpeed", "Set by caller tag for buffing player speed")
	}
	
	namespace GameplayCues
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitscanWeaponImpact, "GameplayCue.Weapon.Hitscan.Impact", "Gameplay cue tag for hitscan weapon impact")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitscanWeaponFire, "GameplayCue.Weapon.Hitscan.Fire", "Gameplay cue tag for hitscan weapon fire")
	}
}