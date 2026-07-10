// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/Abilities/Player/RevivePlayer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Comply.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/ComplyPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void URevivePlayer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	AComplyPlayerCharacter* Avatar = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!Avatar) { EndAbility(Handle, ActorInfo, ActivationInfo, true, false); return; }

	AActor* Owner = GetOwningActorFromActorInfo();

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
		FCollisionShape Sphere = FCollisionShape::MakeSphere(ReviveSphereTraceRadius);
		GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Interact, Sphere, QueryParams);
		
		if (AComplyPlayerCharacter* HitPlayer = Cast<AComplyPlayerCharacter>(Hit.GetActor()))
		{
			TargetPlayer = HitPlayer;
			Reviver = Cast<AComplyPlayerCharacter>(ActorInfo->AvatarActor.Get());

			if (Reviver && TargetPlayer)
			{
				// Rotates the reviver towards the player they are reviving on the server
				Reviver->Server_FaceTarget(TargetPlayer);
				
				// Locally rotates towards the player they are reviving
				FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
					Reviver->GetActorLocation(), TargetPlayer->GetActorLocation());
				Reviver->SetActorRotation(FRotator(0.f, LookAt.Yaw, 0.f));

				APlayerController* PC = Cast<APlayerController>(Avatar->GetController());
				if (PC)
				{
				 	PC->SetIgnoreMoveInput(true);
				}
			}
		}
	}
	
	// After 5 seconds, revive the player. This ability is canceled if input is released before 5 seconds are up
	UAbilityTask_WaitDelay* WaitTask = UAbilityTask_WaitDelay::WaitDelay(this, ReviveTime);
	WaitTask->OnFinish.AddDynamic(this, &URevivePlayer::OnHoldComplete);
	WaitTask->ReadyForActivation();
}

void URevivePlayer::OnHoldComplete()
{
	if (Reviver && TargetPlayer->bIsDowned)
	{
		Reviver->Server_ReviveTarget(TargetPlayer);
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void URevivePlayer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	AComplyPlayerCharacter* Avatar = Cast<AComplyPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (Avatar)
	{
		APlayerController* PC = Cast<APlayerController>(Avatar->GetController());
		if (PC)
		{
			PC->ResetIgnoreMoveInput();

			if (Reviver)
			{
				Reviver->SetActorRotation(FRotator(0.f, PC->GetControlRotation().Yaw, 0.f));
			}
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
