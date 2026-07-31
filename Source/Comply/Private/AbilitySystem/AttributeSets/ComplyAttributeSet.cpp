// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "Framework/GameMode/ComplyGameModeBase.h"
#include "GameplayEffectExtension.h"
#include "Interface/CombatantInterface.h"
#include "Interface/TargetableInterface.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "Interface/Player/PlayerInterface.h"
#include "Net/UnrealNetwork.h"


void UComplyAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// GAS attributes are set to replicate with no condition for replication, and to replicate whenever an attribute is received from the server, even when there's no value change
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxArmor, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxArmorPenetration, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MovementSpeed, COND_None, REPNOTIFY_Always);
}

void UComplyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxArmor());
	}
	
	if (Attribute == GetArmorPenetrationAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxArmorPenetration());
	}
}

void UComplyAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	/** 
	 * Lowers health by the incoming damage meta attribute and clamps again
	 * The meta attribute is stored in a local variable and set to 0 at the start so the correct damage is applied. We also check if the damage is fatal
	 */
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Data);
	}
	
	// Clamps the attribute properly whenever a gameplay effect modifies it
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	
	if (Data.EvaluatedData.Attribute == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), 0.f, GetMaxArmor()));
	}
	
	if (Data.EvaluatedData.Attribute == GetArmorPenetrationAttribute())
	{
		SetArmorPenetration(FMath::Clamp(GetArmorPenetration(), 0.f, GetMaxArmorPenetration()));
	}
}

void UComplyAttributeSet::HandleIncomingDamage(const struct FGameplayEffectModCallbackData& Data)
{
	const float LocalIncomingDamage = GetIncomingDamage();
    SetIncomingDamage(0);
    if (LocalIncomingDamage <= 0) return;

    const AComplyGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AComplyGameModeBase>();

    const AActor* SourceAvatarActor = nullptr;
    const AActor* TargetAvatarActor = nullptr;

    const UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();
    if (SourceASC && SourceASC->AbilityActorInfo && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
    {
        SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
    }
    if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
    {
        TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
    }

	if (GameMode && !GameMode->bFriendlyFire && SourceAvatarActor)
	{
		// When friendly fire is off, players can't damage anything except enemies
		if (SourceAvatarActor->Implements<UPlayerInterface>() && 
			TargetAvatarActor && !TargetAvatarActor->Implements<UEnemyInterface>())
		{
			return;
		}
	}

    // Damage is valid, apply it
    const float NewHealth = GetHealth() - LocalIncomingDamage;
    SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

    UE_LOG(LogTemp, Warning, TEXT("Health after damage: %f"), GetHealth());

    AActor* AvatarActor = Data.Target.GetAvatarActor();

    if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(AvatarActor))
    {
        Combatant->HandleHit(AvatarActor);
    }

    if (AvatarActor && AvatarActor->Implements<UTargetableInterface>())
    {
        ITargetableInterface::Execute_TakeDamage(AvatarActor);
    }

    // If fatal damage
    if (NewHealth <= 0)
    {
    	if (ICombatantInterface* Combatant = Cast<ICombatantInterface>(AvatarActor))
    	{
    		Combatant->Die(AvatarActor);
    	}

        if (AvatarActor && AvatarActor->Implements<UTargetableInterface>())
        {
            ITargetableInterface::Execute_Die(AvatarActor);
        }
    }
}

// In OnRep functions for GAS attributes, a specific GAS macro is used to that GAS handles replication and local prediction on its own
void UComplyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UComplyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UComplyAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Armor, OldArmor);
}

void UComplyAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxArmor, OldArmor);
}

void UComplyAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ArmorPenetration, OldArmorPenetration);
}

void UComplyAttributeSet::OnRep_MaxArmorPenetration(const FGameplayAttributeData& OldArmorPenetration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxArmorPenetration, OldArmorPenetration);
}

void UComplyAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MovementSpeed, OldMovementSpeed);
}
