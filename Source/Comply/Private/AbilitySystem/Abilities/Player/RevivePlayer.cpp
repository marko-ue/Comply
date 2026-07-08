// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/RevivePlayer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Comply.h"
#include "AbilitySystem/ComplyTags.h"
#include "Character/ComplyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void URevivePlayer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Avatar = GetAvatarActorFromActorInfo();

	if (!Avatar || !Owner) return;
	
	FVector2D ViewportSize = FVector2D();
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(
		this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		
		if (Avatar)
		{
			float DistanceToCharacter = (Avatar->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100);
		}
		
		FVector End = Start + CrosshairWorldDirection * 250;
		
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
		
		FHitResult Hit;
		FCollisionShape Sphere = FCollisionShape::MakeSphere(50.f);
		GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Interact, Sphere, QueryParams);

		if (AComplyPlayerCharacter* HitPlayer = Cast<AComplyPlayerCharacter>(Hit.GetActor()))
		{
			AComplyPlayerCharacter* Reviver = Cast<AComplyPlayerCharacter>(ActorInfo->AvatarActor.Get());
			{
				if (HitPlayer->bIsDowned)
				{
					//HitPlayer->bIsDowned = false;
					Reviver->Server_ReviveTarget(HitPlayer);
				}
			}
		}
	}
}
