// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/RangedWeaponAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystem/ComplyAttributeSet.h"

class UComplyAttributeSet;

void URangedWeaponAbilityBase::TraceToCrosshair(FHitResult& TraceHitResult, float TraceLength)
{
	const FGameplayAbilityActivationInfo ActivationInfo;
	AActor* Owner = GetOwningActorFromActorInfo();
	AActor* Avatar = GetAvatarActorFromActorInfo();

	if (!Avatar || !Owner) return;
	
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	
	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
																		 CrosshairLocation,
																		 CrosshairWorldPosition,
																		 CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		Start = CrosshairWorldPosition;
		
		if (Avatar)
		{
			float DistanceToCharacter = (Avatar->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100);
		}
		
		End = Start + CrosshairWorldDirection * TraceLength;

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Avatar);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECC_Pawn,
			CollisionParams
		);
		
		if (bHit) 
		{
			const UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TraceHitResult.GetActor());
			if (TargetASC)
			{
				//if (HasAuthority(&ActivationInfo))
				//{
					CauseDamage(TraceHitResult.GetActor());
				//}
			}
		}
	}
}
