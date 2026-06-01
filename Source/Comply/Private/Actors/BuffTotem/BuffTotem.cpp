// Copyright © 2026 Marko. All rights reserved.


#include "Actors/BuffTotem/BuffTotem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/ComplyTags.h"
#include "Components/SphereComponent.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "Interface/Player/PlayerInterface.h"


ABuffTotem::ABuffTotem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("BeaconMesh"));
	SetRootComponent(StaticMeshComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComp->SetupAttachment(GetRootComponent());
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
}

void ABuffTotem::BeginPlay()
{
	Super::BeginPlay();
}

void ABuffTotem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABuffTotem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// This event will just be used to apply effects and play sounds when a totem buff is picked up 
			ASC->RegisterGameplayTagEvent(
					ComplyTags::States::State_TotemBuffed,
					EGameplayTagEventType::AnyCountChange
				).AddUObject(this, &ABuffTotem::OnTotemBuffTagChanged);

			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ApplyTotemBuffEffectClass, 1.f, ContextHandle);
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			
			// After the totem applied its buff (gameplay effect) to 3 actors in total, destroy it
			BuffCount++;
			if (BuffCount >= 3)
			{
				Destroy();
			}
		}
	}
}

void ABuffTotem::OnTotemBuffTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// TODO: Effects and sounds
}
