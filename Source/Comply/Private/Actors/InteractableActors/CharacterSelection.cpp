// Copyright © 2026 Marko. All rights reserved.

#include "Actors/InteractableActors/CharacterSelection.h"

// Comply
#include "ComplyPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterSelection)

ACharacterSelection::ACharacterSelection()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ACharacterSelection::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterSelection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACharacterSelection::Interact(APlayerController* InstigatorPC)
{
	AComplyPlayerController* PC = Cast<AComplyPlayerController>(InstigatorPC);
	if (!PC) return;
	PC->OpenMenuWidget(CharacterSelectionWidgetClass);
}
