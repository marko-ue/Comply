// Copyright © 2026 Marko. All rights reserved.

#include "Character/Player/EnforcerCharacter.h"

// UE
#include "CableComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnforcerCharacter)

AEnforcerCharacter::AEnforcerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GrappleAnchorPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GrappleAnchorPoint"));
	GrappleAnchorPoint->SetupAttachment(RootComponent);
	
	GrappleCable = CreateDefaultSubobject<UCableComponent>(TEXT("GrappleCable"));
	GrappleCable->SetupAttachment(GetMesh(), FName("weapon_r")); // Tip of the hook device
	GrappleCable->SetVisibility(false); // Hidden until fired
	GrappleCable->bAttachEnd = false; // The end point will be set manually
	GrappleCable->CableLength = 0.f;
	GrappleCable->NumSegments = 1;
	GrappleCable->SolverIterations = 8;
}


