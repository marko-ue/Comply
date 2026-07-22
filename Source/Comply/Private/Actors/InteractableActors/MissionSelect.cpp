// Copyright © 2026 Marko. All rights reserved.


#include "Actors/InteractableActors/MissionSelect.h"

#include "ComplyPlayerController.h"


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

