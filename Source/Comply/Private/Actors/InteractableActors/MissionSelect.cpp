// Copyright © 2026 Marko. All rights reserved.

#include "Actors/InteractableActors/MissionSelect.h"

// Comply
#include "ComplyPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MissionSelect)

AMissionSelect::AMissionSelect()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AMissionSelect::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMissionSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMissionSelect::Interact(APlayerController* InstigatorPC)
{
	AComplyPlayerController* PC = Cast<AComplyPlayerController>(InstigatorPC);
	if (!PC) return;
	PC->OpenMenuWidget(MissionSelectWidgetClass);
}

