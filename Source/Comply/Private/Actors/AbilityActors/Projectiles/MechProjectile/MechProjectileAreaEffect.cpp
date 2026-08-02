// Copyright © 2026 Marko. All rights reserved.


#include "Actors/AbilityActors/Projectiles/MechProjectile/MechProjectileAreaEffect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ComplyAbilityTypes.h"
#include "AbilitySystem/Data/Enemy/Abilities/Mech/MechProjectileData.h"
#include "AbilitySystem/Data/Enemy/Damage/ComplyEnemyDamageData.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/Player/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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

void AMechProjectileAreaEffect::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ProjectileData);
}

void AMechProjectileAreaEffect::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(ProjectileData, TEXT("ProjectileData not passed into %s"), *GetName());
	
	AreaEffectMesh->SetStaticMesh(ProjectileData->AreaEffectMesh);
	AreaEffectMesh->SetMaterial(0, ProjectileData->AreaEffectMaterial);
	
	AreaEffectNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileData->AreaEffectParticles, GetActorLocation());
	AreaEffectAudioComponent = UGameplayStatics::SpawnSoundAttached(ProjectileData->AreaEffectSound, RootComponent);
	SetLifeSpan(4.5f);
}

void AMechProjectileAreaEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMechProjectileAreaEffect::ApplyEffectToTarget(AActor* OverlappingActor, UAbilitySystemComponent* TargetASC)
{
	// Adds the already overlapping actor so the same effect is not applied multiple times on the same actor
	if (AffectedActors.Contains(OverlappingActor)) return;
	AffectedActors.Add(OverlappingActor);

	ApplyDamageToTarget(TargetASC);
	ApplySlowToTarget(TargetASC);
}

void AMechProjectileAreaEffect::ApplyDamageToTarget(UAbilitySystemComponent* TargetASC)
{
	FComplyGameplayEffectContext* Context = new FComplyGameplayEffectContext();
	FGameplayEffectContextHandle ContextHandle(Context);
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ProjectileData->EnemyDamageData->DamageEffectClass, 1.f, ContextHandle);
	if (!SpecHandle.IsValid()) return;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, ProjectileData->EnemyDamageData->DamageType, ProjectileData->EnemyDamageData->Damage.GetValueAtLevel(1.f)
	);

	ActiveDamageEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

void AMechProjectileAreaEffect::ApplySlowToTarget(UAbilitySystemComponent* TargetASC)
{
	if (!SourceASC || !TargetASC || !ProjectileData->SlowEffectClass) return;

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceASC->GetAvatarActor());

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(ProjectileData->SlowEffectClass, 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		ActiveSlowEffectHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void AMechProjectileAreaEffect::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const bool bIsPlayer = OtherActor->Implements<UPlayerInterface>();
	const bool bIsTargetable = OtherActor->Implements<UTargetableInterface>();
    
	if (!bIsPlayer && !bIsTargetable) return;

	if (bIsPlayer)
	{
		// Immediately apply speed reduction on the local client
		if (AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(OtherActor))
		{
			if (PlayerCharacter->IsLocallyControlled() && !PlayerCharacter->HasAuthority() && PlayerCharacter->GetCharacterMovement())
			{
				PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed -= 300.f;
				PlayerCharacter->NextSlowMagnitude = 300.f;
			}
		}
	}

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (!SourceASC || !TargetASC || !ProjectileData->EnemyDamageData->DamageEffectClass) return;
			ApplyEffectToTarget(OtherActor, TargetASC);
		}
	}
}

void AMechProjectileAreaEffect::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AffectedActors.Remove(OtherActor);
	
	// Immediately add back speed on the local client
	if (const AComplyPlayerCharacter* PlayerCharacter = Cast<AComplyPlayerCharacter>(OtherActor))
	{
		if (PlayerCharacter->IsLocallyControlled() && !PlayerCharacter->HasAuthority() && PlayerCharacter->GetCharacterMovement())
		{
			PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed += 300.f;
		}
	}
	
	if (const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
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

