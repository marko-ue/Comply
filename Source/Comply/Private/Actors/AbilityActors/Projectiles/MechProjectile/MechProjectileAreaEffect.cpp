// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectileAreaEffect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "Interface/Player/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AMechProjectileAreaEffect::AMechProjectileAreaEffect()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AreaEffectMesh = CreateDefaultSubobject<UStaticMeshComponent>("AreaEffectMesh");
	SetRootComponent(AreaEffectMesh);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetupAttachment(GetRootComponent());
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnComponentEndOverlap);
}

void AMechProjectileAreaEffect::BeginPlay()
{
	Super::BeginPlay();
	
	AreaEffectNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AreaEffectParticles, GetActorLocation());
	AreaEffectAudioComponent = UGameplayStatics::SpawnSoundAttached(AreaEffectSound, RootComponent);
	SetLifeSpan(4.5f);
}

void AMechProjectileAreaEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMechProjectileAreaEffect::ApplyEffectToTarget(UAbilitySystemComponent* TargetASC)
{
	if (!SourceASC || !TargetASC || !DamageEffectClass) return;
	
	if (AffectedActors.Contains(TargetActor)) return;
	AffectedActors.Add(TargetActor);
	
	ApplyDamageToTarget(TargetASC);
	ApplySlowToTarget(TargetASC);
}

void AMechProjectileAreaEffect::ApplyDamageToTarget(UAbilitySystemComponent* TargetASC)
{
	FComplyGameplayEffectContext* Context = new FComplyGameplayEffectContext();
	FGameplayEffectContextHandle ContextHandle(Context);
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, ContextHandle);
	if (!SpecHandle.IsValid()) return;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ExplicitDamage.GetValueAtLevel(1.f));

	ActiveDamageEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void AMechProjectileAreaEffect::ApplySlowToTarget(UAbilitySystemComponent* TargetASC)
{
	if (!SourceASC || !TargetASC || !SlowEffectClass) return;

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceASC->GetAvatarActor());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(SlowEffectClass, 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		ActiveSlowEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void AMechProjectileAreaEffect::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UPlayerInterface>()) return;

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (!SourceASC || !TargetASC || !DamageEffectClass) return;

			ApplyEffectToTarget(TargetASC);
		}
	}
}

void AMechProjectileAreaEffect::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AffectedActors.Remove(OtherActor);
	
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (TargetASC)
			{
				if (ActiveDamageEffectHandle.IsValid() && ActiveSlowEffectHandle.IsValid())
				{
					TargetASC->RemoveActiveGameplayEffect(ActiveDamageEffectHandle);
					TargetASC->RemoveActiveGameplayEffect(ActiveSlowEffectHandle);
				}
			}
		}
	}
}

void AMechProjectileAreaEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AreaEffectAudioComponent)
	{
		AreaEffectAudioComponent->FadeOut(3.f, 0.f);
	}
	
	if (AreaEffectNiagaraComponent)
	{
		AreaEffectNiagaraComponent->Deactivate();
	}
	
	Super::EndPlay(EndPlayReason);
}

