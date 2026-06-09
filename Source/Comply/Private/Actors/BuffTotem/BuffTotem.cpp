// Copyright © 2026 Marko. All rights reserved.


#include "Actors/BuffTotem/BuffTotem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Interface/Player/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


ABuffTotem::ABuffTotem()
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

void ABuffTotem::BeginPlay()
{
	Super::BeginPlay();
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PlaceTotemImpactParticles, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TotemPlaceSound, GetActorLocation());
	
	TotemNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, TotemParticles, GetActorLocation());
	HumAudioComponent = UGameplayStatics::SpawnSoundAttached(TotemHummingSound, GetRootComponent());
}

void ABuffTotem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABuffTotem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	// If the actor beginning overlap is already overlapping, don't apply the buff 
	if (OverlappingActors.Contains(OtherActor)) return;
	
	if (OtherActor->Implements<UPlayerInterface>())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			OverlappingActors.Add(OtherActor);
			
			FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ApplyTotemBuffEffectClass, 1.f, ContextHandle);
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			
			FGameplayCueParameters CueParams;
			CueParams.Location = OtherActor->GetActorLocation();
			ASC->ExecuteGameplayCue(ComplyTags::GameplayCues::BuffTotemApplyBuff, CueParams);
			
			// After the totem applied its buff (gameplay effect) to 3 actors in total, destroy it
			BuffCount++;
			if (BuffCount >= 3)
			{
				Destroy();
			}
		}
	}
}

void ABuffTotem::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Remove the actor so it can overlap again
	OverlappingActors.Remove(OtherActor);
}

void ABuffTotem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HumAudioComponent)
	{
		HumAudioComponent->FadeOut(2.f, 0.f);
	}
	
	if (TotemNiagaraComponent)
	{
		TotemNiagaraComponent->Deactivate();
	}
	
	Super::EndPlay(EndPlayReason);
}
