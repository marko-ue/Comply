// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAttributeSet.h"
#include "Framework/GameMode/ComplyGameModeBase.h"
#include "GameplayEffectExtension.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"
#include "Interface/Player/PlayerInterface.h"
#include "Net/UnrealNetwork.h"


void UComplyAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// GAS attributes are set to replicate with no condition for replication, and to replicate whenever an attribute is received from the server, even when there's no value change
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UComplyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UComplyAttributeSet::HandleIncomingDamage(const struct FGameplayEffectModCallbackData& Data)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0);
	if (LocalIncomingDamage > 0)
	{
		AComplyGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AComplyGameModeBase>();
		if (GameMode)
		{
			// If friendly fire is off, and the source and target of the gameplay effect context both implement the player interface, damage won't be dealt
			if (GameMode->bFriendlyFire == false)
			{
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
			
				if (SourceAvatarActor && TargetAvatarActor)
				{
					if (SourceAvatarActor->Implements<UPlayerInterface>() && TargetAvatarActor->Implements<UPlayerInterface>()) return;
				}
			}
		}
		
	   /** 
		* Clamps the health attribute to a minimum of 0 and maximum of max health
		* This doesn't permanently clamp, and instead just changes the value returned from querying the modification of the attribute instead of actually setting health
		* This means that further calculations will break the clamp, so clamping should also be done in PostGameplayEffectExecute
		* If friendly fire is on, damage will be dealt no matter who's attacking and being attacked
		*/
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			
		UE_LOG(LogTemp, Warning, TEXT("Health after damage: %f"), GetHealth());
			
		const bool bFatal = NewHealth <= 0.f;
		if (bFatal)
		{
			Data.Target.GetAvatarActor()->Destroy();
		}
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
