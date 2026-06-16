// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/ReloadAbilities/ShotgunReload.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "Character/ComplyPlayerCharacter.h"


void UShotgunReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
    if (Character)
    {
        ActiveWeapon = Character->GetEquippedPrimaryWeapon();
        Character->bIsReloading = true;
    }
    
    if (!ActiveWeapon)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // Apply reloading tag
    FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ReloadStateEffectClass, 1.f, ContextHandle);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    // Listen for firing tag so reload can be interrupted when the player shoots
    WaitFiringTagTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, ComplyTags::States::State_Firing);
    WaitFiringTagTask->Added.AddDynamic(this, &UShotgunReload::OnFiringTagAdded);
    WaitFiringTagTask->ReadyForActivation();

    LoadNextShell();
}

void UShotgunReload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    AComplyPlayerCharacter* Character = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
    if (Character)
    {
        Character->bIsReloading = false;
    }
    
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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
        FGameplayTagContainer Tags;
        Tags.AddTag(ComplyTags::States::State_Reloading);
        GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);
        
        FGameplayCueParameters CueParams;
        CueParams.Location = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo())->WeaponMesh->GetComponentLocation();
        CueParams.Instigator = GetAvatarActorFromActorInfo();
        GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ComplyTags::GameplayCues::WeaponReloadFinished, CueParams);

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    ShellMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this, NAME_None, ActiveWeapon->InsertShellMontage, 1.f, NAME_None, true);
    
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
    FGameplayEffectContextHandle AmmoContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    FGameplayEffectSpecHandle AmmoSpec = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(AddAmmoEffectClass, 1.f, AmmoContext);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*AmmoSpec.Data.Get());

    // Deduct 1 from reserve
    FGameplayEffectContextHandle ReserveContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    FGameplayEffectSpecHandle ReserveSpec = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(ActiveWeapon->ReduceReserveAmmoEffectClass, 1.f, ReserveContext);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(ReserveSpec, ActiveWeapon->GetReduceReserveAmmoTag(), -1.f);
    GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*ReserveSpec.Data.Get());

    // Try to load the next shell
    LoadNextShell();
}

void UShotgunReload::OnFiringTagAdded()
{
    if (!IsActive() || !IsValid(this))
    {
        return;
    }
    
    // Firing interrupted the reload, end the montage, remove the reloading tag, and end the ability
    if (ShellMontageTask)
    {
        ShellMontageTask->EndTask();
        ShellMontageTask = nullptr;
    }

    FGameplayTagContainer Tags;
    Tags.AddTag(ComplyTags::States::State_Reloading);
    GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(Tags);

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}