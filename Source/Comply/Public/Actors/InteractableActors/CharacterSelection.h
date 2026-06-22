// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActorBase.h"
#include "GameFramework/Actor.h"
#include "Interface/Player/InteractableInterface.h"
#include "CharacterSelection.generated.h"

UCLASS()
class COMPLY_API ACharacterSelection : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	ACharacterSelection();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact(APlayerController* InstigatorPC) override;


protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> CharacterSelectionWidgetClass;
	
};
