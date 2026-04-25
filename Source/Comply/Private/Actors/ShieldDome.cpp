// Copyright © 2026 Marko. All rights reserved.

#include "Actors/ShieldDome.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/ComplyTags.h"
#include "Interface/Player/PlayerInterface.h"
#include "Components/SphereComponent.h"

AShieldDome::AShieldDome()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("ShieldMesh"));
	SetRootComponent(StaticMeshComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComp->SetupAttachment(GetRootComponent());
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
}

void AShieldDome::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

		if (ASC)
		{
			if (OtherActor->Implements<UPlayerInterface>())
			{
				ASC->TryActivateAbilityByClass(ApplyShieldedEffectAbilityClass);
			}
		}
	}
}

void AShieldDome::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();

		if (ASC)
		{
			if (OtherActor->Implements<UPlayerInterface>())
			{
				FGameplayTagContainer Tag;
				Tag.AddTag(ComplyTags::ComplyAbilities::AssetTags::ApplyShieldedEffectAbility);
				ASC->CancelAbilities(&Tag);
			}
		}
	}
}


