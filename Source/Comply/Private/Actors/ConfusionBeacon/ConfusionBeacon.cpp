// Copyright © 2026 Marko. All rights reserved.


#include "Actors/ConfusionBeacon/ConfusionBeacon.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/ComplyTags.h"
#include "Components/SphereComponent.h"
#include "Interface/Enemy/EnemyInterface.h"
#include "Interface/Player/PlayerInterface.h"


AConfusionBeacon::AConfusionBeacon()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("BeaconMesh"));
	SetRootComponent(StaticMeshComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComp->SetupAttachment(GetRootComponent());
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
}

void AConfusionBeacon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AConfusionBeacon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

		if (ASC)
		{
			if (OtherActor->Implements<UEnemyInterface>())
			{
				// TODO: Fix ability not applying. Will be fixed once the confused effect can actually be used (when enemies are able to attack)
				ASC->TryActivateAbilityByClass(ApplyConfusedEffectAbilityClass);
			}
		}
	}
}

void AConfusionBeacon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

		if (ASC)
		{
			if (OtherActor->Implements<UEnemyInterface>())
			{
				FGameplayTagContainer Tag;
				Tag.AddTag(ComplyTags::ComplyAbilities::AssetTags::ApplyConfusedEffectAbility);
				ASC->CancelAbilities(&Tag);
			}
		}
	}
}

void AConfusionBeacon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

