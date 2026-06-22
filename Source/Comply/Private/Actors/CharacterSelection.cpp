// Copyright © 2026 Marko. All rights reserved.


#include "Actors/CharacterSelection.h"


// Sets default values
ACharacterSelection::ACharacterSelection()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACharacterSelection::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterSelection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

