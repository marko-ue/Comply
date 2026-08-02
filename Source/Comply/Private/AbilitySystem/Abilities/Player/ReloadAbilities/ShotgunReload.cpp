// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/ReloadAbilities/ShotgunReload.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Character/ComplyPlayerCharacter.h"


void UShotgunReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
    checkf(ActiveWeapon->WeaponData, TEXT("WeaponData not set on %s"), *GetName());
    
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    
    if (AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo()))
    {
        ActiveWeapon = Character->GetEquippedPrimaryWeapon();
        Character->bIsReloading = true;
    }
    
    if (!ActiveWeapon)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }
    
    bool bFound = false;
    const float CurrentAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentAmmoAttribute(), bFound);
    const float MaxAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetMaxAmmoAttribute(), bFound);
    
    // Don't attempt to load the next shell if mag is full
    if (CurrentAmmo >= MaxAmmo)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // Apply reloading tag
    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReloadStateEffectClass, 1.f, ContextHandle);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    LoadNextShell();
}

void UShotgunReload::LoadNextShell()
{
    // Don't run if ability is already ending
    if (!IsActive())
    {
        return;
    }
    
    bool bFound = false;
    const float CurrentAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentAmmoAttribute(), bFound);
    const float MaxAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetMaxAmmoAttribute(), bFound);
    const float ReserveAmmo = GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValue(ActiveWeapon->GetCurrentReserveAmmoAttribute(), bFound);

    // Stop if mag is full or no reserve left
    if (CurrentAmmo >= MaxAmmo || ReserveAmmo <= 0.f)
    {
        FGameplayCueParameters CueParams;
        CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
        CueParams.Instigator = GetAvatarActorFromActorInfo();
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::WeaponReloadFinished, CueParams);
        
        GetAbilitySystemComponentFromActorInfo()->SetLooseGameplayTagCount(ComplyTags::States::State_FiringBlocked, 0);

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    ShellMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, ActiveWeapon->WeaponData->InsertShellMontage, 1.f, NAME_None, true);
    
    // Always validate the task before attempting to bind
    if (!IsValid(ShellMontageTask))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }

    if (IsValid(ShellMontageTask))
    {
        ShellMontageTask->OnCompleted.AddDynamic(this, &UShotgunReload::OnShellMontageCompleted);
        ShellMontageTask->ReadyForActivation();
        
        FGameplayCueParameters CueParams;
        CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
        CueParams.Instigator = GetAvatarActorFromActorInfo();
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::WeaponReload, CueParams);
    }
}

void UShotgunReload::OnShellMontageCompleted() 
{
    if (!IsActive() || !IsValid(this))
    {
        return;
    }
    
    // Add 1 shell to mag
    const FGameplayEffectContextHandle AmmoContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    const FGameplayEffectSpecHandle AmmoSpec = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(AddAmmoEffectClass, 1.f, AmmoContext);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*AmmoSpec.Data.Get());

    // Deduct 1 from reserve
    const FGameplayEffectContextHandle ReserveContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    const FGameplayEffectSpecHandle ReserveSpec = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ActiveWeapon->WeaponData->ReduceReserveAmmoEffectClass, 1.f, ReserveContext);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(ReserveSpec, ActiveWeapon->GetReduceReserveAmmoTag(), -1.f);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ReserveSpec.Data.Get());

    // Try to load the next shell
    LoadNextShell();
}

void UShotgunReload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo()))
    {
        Character->bIsReloading = false;
    }
    
    // Also remove the tags here so the removal replicates to clients
    FGameplayTagContainer Tags;
    Tags.AddTag(ComplyTags::States::State_Reloading);
    GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);
    
    GetAbilitySystemComponentFromActorInfo()->SetLooseGameplayTagCount(ComplyTags::States::State_FiringBlocked, 0);
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
