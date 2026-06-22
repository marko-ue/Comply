// Copyright © 2026 Marko. All rights reserved.


#include "Actors/InteractableActors/InteractableActorBase.h"

#include "Components/WidgetComponent.h"


AInteractableActorBase::AInteractableActorBase()
{
	PrimaryActorTick.bCanEverTick = true;
	
	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	SetRootComponent(InteractableMesh);
	
	PromptWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidget"));
	PromptWidgetComponent->SetupAttachment(RootComponent);
	PromptWidgetComponent->SetVisibility(false);
}

void AInteractableActorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableActorBase::Interact(APlayerController* InstigatorPC)
{
	
}

void AInteractableActorBase::ShowInteractionPrompt()
{
	PromptWidgetComponent->SetVisibility(true);
}

void AInteractableActorBase::HideInteractionPrompt()
{
	PromptWidgetComponent->SetVisibility(false);
}
