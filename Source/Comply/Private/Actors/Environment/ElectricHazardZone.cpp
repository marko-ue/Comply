// Copyright © 2026 Marko. All rights reserved.

#include "Actors/Environment/ElectricHazardZone.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueNotifyTypes.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystem/Data/Environment/ElectricHazardZoneData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Player/PlayerInterface.h"


AElectricHazardZone::AElectricHazardZone()
{
	PrimaryActorTick.bCanEverTick = true;
	
	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	SetRootComponent(BoxComp);
	
	ElectricEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ElectricEffect"));
	ElectricEffect->SetupAttachment(RootComponent);

	ElectricSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ElectricSound"));
	ElectricSound->SetupAttachment(RootComponent);
	
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
	
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
}

void AElectricHazardZone::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(ElectricHazardZoneData, TEXT("ElectricHazardZoneData not set on %s"), *GetName());
	
	ElectricEffect->SetAsset(ElectricHazardZoneData->ElectricEffect);
	ElectricSound->SetSound(ElectricHazardZoneData->ElectricSound);
}

void AElectricHazardZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UAbilitySystemComponent* AElectricHazardZone::GetAbilitySystemComponent() const
{
	return ASC;
}

void AElectricHazardZone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;
	
	// Immediately reduce speed on the local client
	if (AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(OtherActor))
	{
		if (PlayerCharacter->IsLocallyControlled() && !PlayerCharacter->HasAuthority() && PlayerCharacter->GetCharacterMovement())
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed -= 200.f;
			PlayerCharacter->NextSlowMagnitude = 200.f;
		}
	}

	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (!GetAbilitySystemComponent() || !TargetASC || !ElectricHazardZoneData->DamageEffectClass) return;

			ApplyEffectToTarget(OtherActor, TargetASC);
			
			// The stun lasts 1 second. It will be reapplied every 1.5 seconds if still in the zone to allow a 0.5 second room without being in a stunned state
			FTimerHandle NewTimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				NewTimerHandle, [this, OtherActor, TargetASC]() { ApplyStunToTarget(OtherActor, TargetASC); }, 1.5f, true
			);
			StunTimerHandles.Add(OtherActor, NewTimerHandle);
		}
	}
}

void AElectricHazardZone::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AffectedActors.Remove(OtherActor);
	
	// Immediately add back speed on the local client
	if (const AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(OtherActor))
	{
		if (PlayerCharacter->IsLocallyControlled() && !PlayerCharacter->HasAuthority() && PlayerCharacter->GetCharacterMovement())
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed += 200.f;
		}
	}
	
	// Clear this actor's stun timer
	if (FTimerHandle* TimerHandle = StunTimerHandles.Find(OtherActor))
	{
		GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
		StunTimerHandles.Remove(OtherActor);
	}
	
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (const FActiveGameplayEffectHandle* DamageHandle = ActiveDamageEffectHandles.Find(OtherActor))
			{
				TargetASC->RemoveActiveGameplayEffect(*DamageHandle);
				ActiveDamageEffectHandles.Remove(OtherActor);
			}

			if (const FActiveGameplayEffectHandle* StunHandle = ActiveStunEffectHandles.Find(OtherActor))
			{
				TargetASC->RemoveActiveGameplayEffect(*StunHandle);
				ActiveStunEffectHandles.Remove(OtherActor);
			}
		}
	}
}

void AElectricHazardZone::ApplyEffectToTarget(AActor* OverlappingActor, UAbilitySystemComponent* TargetASC)
{
	if (!TargetASC || !ElectricHazardZoneData->DamageEffectClass) return;
	
	if (AffectedActors.Contains(OverlappingActor)) return;
	AffectedActors.Add(OverlappingActor);
	
	ApplyDamageToTarget(OverlappingActor, TargetASC);
	ApplyStunToTarget(OverlappingActor, TargetASC);
}

void AElectricHazardZone::ApplyStunToTarget(AActor* OverlappingActor, UAbilitySystemComponent* TargetASC)
{
	if (!GetAbilitySystemComponent() || !TargetASC || !ElectricHazardZoneData->StunEffectClass) return;
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(GetAbilitySystemComponent()->GetAvatarActor());

	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ElectricHazardZoneData->StunEffectClass, 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		const FActiveGameplayEffectHandle Handle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		ActiveStunEffectHandles.Add(OverlappingActor, Handle);
	}
}

void AElectricHazardZone::ApplyDamageToTarget(AActor* OverlappingActor, UAbilitySystemComponent* TargetASC)
{
	FComplyGameplayEffectContext* Context = new FComplyGameplayEffectContext();
	FGameplayEffectContextHandle ContextHandle(Context);
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(ElectricHazardZoneData->DamageEffectClass, 1.f, ContextHandle);
	if (!SpecHandle.IsValid()) return;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ElectricHazardZoneData->DamageType, ElectricHazardZoneData->Damage.GetValueAtLevel(1.f));

	const FActiveGameplayEffectHandle Handle = GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	ActiveDamageEffectHandles.Add(OverlappingActor, Handle);
}
