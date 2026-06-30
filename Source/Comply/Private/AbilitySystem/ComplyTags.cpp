#include "AbilitySystem/ComplyTags.h"

namespace ComplyTags
{
	namespace ComplyAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "ComplyTags.Abilities.AbilityTags.Primary", "Primary ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Utility, "ComplyTags.Abilities.AbilityTags.Utility", "Utility ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Throwable, "ComplyTags.Abilities.AbilityTags.Throwable", "Throwable ability asset tag")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sprint, "ComplyTags.Abilities.AbilityTags.Sprint", "Sprint ability asset tag")
		
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
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input_Sprint, "ComplyTags.Abilities.InputTags.Input_Sprint", "Input tag for sprinting")
		}
		
		namespace DamageTypes
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "ComplyTags.Abilities.DamageTypes.Physical", "Physical damage type")
		}
	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_MeleeAttackRobot, "ComplyTags.Events.Abilities.MeleeAttackRobot", "Event tag for activating the melee attack robot ability")
	}
	
	namespace States
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Aiming, "ComplyTags.States.Aiming", "Is player aiming")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Firing, "ComplyTags.States.Firing", "Is player firing")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Shielded, "ComplyTags.States.Shielded", "Is player being shielded")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Reloading, "ComplyTags.States.Reloading", "Is player reloading")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Distracted, "ComplyTags.States.Distracted", "Is enemy distracted")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Equipping, "ComplyTags.States.Equipping", "Is player equipping")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_TotemBuffed, "ComplyTags.States.TotemBuffed", "Is player buffed by a totem")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Sprinting, "ComplyTags.States.Sprinting", "Is player sprinting")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_ThrowablePreparing, "ComplyTags.States.Throwable.Preparing", "Is player preparing throwable")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_ThrowableThrowing, "ComplyTags.States.Throwable.Throwing", "Is player throwing a throwable")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_NoThrowables, "ComplyTags.States.NoGrenades", "Does player have no throwables")
		
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
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShotgunImpact, "GameplayCue.Weapon.Shotgun.Impact", "Gameplay cue tag for shotgun impact")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitscanWeaponFire, "GameplayCue.Weapon.Hitscan.Fire", "Gameplay cue tag for hitscan weapon fire")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponDryFire, "GameplayCue.Weapon.DryFire", "Gameplay cue tag for when dry firing a weapon")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponReload, "GameplayCue.Weapon.Reload", "Gameplay cue tag for reloading weapons")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(WeaponReloadFinished, "GameplayCue.Weapon.ReloadFinished", "Gameplay cue tag for when reloading of weapons finishes")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShieldPlaced, "GameplayCue.Shield.Placed", "Gameplay cue tag for when the shield is placed")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShieldHitscanWeaponImpact, "GameplayCue.Shield.HitscanWeaponImpact", "Gameplay cue for when a shot passes through a shield")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ShieldShotgunImpact, "GameplayCue.Shield.ShotgunImpact", "Gameplay cue for when a shotgun bullet passes through a shield")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(PullGrenadePin, "GameplayCue.Grenades.PullPin", "Gameplay cue for when a grenade pin is pulled")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ThrowGrenade, "GameplayCue.Grenades.Throw", "Gameplay cue tag for throwing grenades")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ExplodeGrenade, "GameplayCue.Grenades.Explode", "Gameplay cue tag for exploding grenades")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GrapplingHookUse, "GameplayCue.GrapplingHook.Use", "Gameplay cue tag for using grappling hook")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GrapplingHookImpact, "GameplayCue.GrapplingHook.Impact", "Gameplay cue tag for grappling hook impact")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GrapplingHookHooking, "GameplayCue.GrapplingHook.Hooking", "Gameplay cue tag for hooking")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TurretTyping, "GameplayCue.Turret.Typing", "Gameplay cue tag for the typing before placing a turret")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(TurretImpact, "GameplayCue.Turret.Impact", "Gameplay cue tag for turret shot impact")
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BuffTotemApplyBuff, "GameplayCue.BuffTotem.ApplyBuff", "Gameplay cue tag for when a buff totem buff is applied")
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(BuffTotemBuffAura, "GameplayCue.BuffTotem.BuffAura", "Gameplay cue tag for the buff aura when a buff exists")
	}
}