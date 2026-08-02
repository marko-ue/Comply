// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/BuffTotem/BuffTotem.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/Data/Player/Abilities/Utilities/BuffTotemUtilityData.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Interface/Player/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


ABuffTotem::ABuffTotem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("BuffTotemMesh"));
	SetRootComponent(StaticMeshComp);
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(FName("Sphere"));
	SphereComp->SetupAttachment(GetRootComponent());
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnOverlapEnd);
}

void ABuffTotem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ABuffTotem, BuffTotemData);
}

void ABuffTotem::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(BuffTotemData, TEXT("BuffTotemData not passed into %s"), *GetName());
	
	StaticMeshComp->SetStaticMesh(BuffTotemData->UtilityMesh);
	StaticMeshComp->SetMaterial(0, BuffTotemData->UtilityMaterial);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BuffTotemData->PlaceTotemImpactParticles, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), BuffTotemData->TotemPlaceSound, GetActorLocation());
	
	TotemNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BuffTotemData->TotemParticles, GetActorLocation());
	HumAudioComponent = UGameplayStatics::SpawnSoundAttached(BuffTotemData->TotemHummingSound, GetRootComponent());
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
			
			const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
			const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BuffTotemData->ApplyTotemBuffEffectClass, 1.f, ContextHandle);
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
