// Copyright © 2026 Marko. All rights reserved.

#include "Actors/AbilityActors/ShieldDome/ShieldDome.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "Components/AudioComponent.h"
#include "Interface/Player/PlayerInterface.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AShieldDome::AShieldDome()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	
	ShieldStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("ShieldMesh"));
	SetRootComponent(ShieldStaticMeshComp);
	
	ShieldObjectStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("ShieldObjectMesh"));
	ShieldObjectStaticMeshComp->SetupAttachment(ShieldStaticMeshComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComp->SetupAttachment(GetRootComponent());
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
}

void AShieldDome::BeginPlay()
{
	Super::BeginPlay();
	
	// The looping effects and sounds for the shield actor are handled at BeginPlay since the actor is replicated, no need for a cue
	ShieldHumNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ShieldParticles, GetActorLocation());
	HumAudioComponent = UGameplayStatics::SpawnSoundAttached(ShieldHummingSound, RootComponent);
}

void AShieldDome::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
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
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
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

void AShieldDome::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HumAudioComponent)
	{
		HumAudioComponent->FadeOut(3.f, 0.f);
	}
	
	if (ShieldHumNiagaraComponent)
	{
		ShieldHumNiagaraComponent->Deactivate();
	}
	
	Super::EndPlay(EndPlayReason);
	
}
