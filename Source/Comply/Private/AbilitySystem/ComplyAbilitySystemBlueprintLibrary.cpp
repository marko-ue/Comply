// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/ComplyAbilityTypes.h"
#include "Kismet/GameplayStatics.h"

FGameplayAbilityTargetDataHandle UComplyAbilitySystemBlueprintLibrary::GetShotgunTargetData(
	const FGameplayCueParameters& Parameters)
{
	const FComplyGameplayEffectContext* Context = static_cast<const FComplyGameplayEffectContext*>(Parameters.EffectContext.Get());
	if (Context)
	{
		return Context->ShotgunTracesTargetData;
	}
	return FGameplayAbilityTargetDataHandle();
}

int32 UComplyAbilitySystemBlueprintLibrary::GetShotgunTargetDataNum(const FGameplayAbilityTargetDataHandle& Handle)
{
	return Handle.Num();
}

bool UComplyAbilitySystemBlueprintLibrary::GetCrosshairTraceStartEnd(const UObject* WorldContextObject,
	const AActor* Avatar, const float TraceLength, FVector& OutStart, FVector& OutEnd, FVector& OutDirection)
{
	FVector2D ViewportSize = FVector2D();
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	const FVector2D CrosshairLocation(ViewportSize.X / 2, ViewportSize.Y / 2);
	FVector CrosshairWorldPosition;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0),
		CrosshairLocation, CrosshairWorldPosition, OutDirection
	);

	if (!bScreenToWorld) return false;

	OutStart = CrosshairWorldPosition;
	const float DistanceToCharacter = (Avatar->GetActorLocation() - OutStart).Size();
	OutStart += OutDirection * (DistanceToCharacter + 25.f);
	OutEnd = OutStart + OutDirection * TraceLength;

	return true;
}

