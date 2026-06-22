// Copyright © 2026 Marko. All rights reserved.


#include "Actors/CharacterSelection.h"

#include "ComplyPlayerController.h"


ACharacterSelection::ACharacterSelection()
{
	PrimaryActorTick.bCanEverTick = true;
	
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	SetRootComponent(InteractableMesh);
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
