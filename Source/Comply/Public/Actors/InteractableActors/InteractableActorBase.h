// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Interface/Player/InteractableInterface.h"

#include "InteractableActorBase.generated.h"

class UWidgetComponent;

/**
 * Base class for interactable actors.
 * Contains some common functions and variables.
 */
UCLASS(Abstract)
class COMPLY_API AInteractableActorBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AInteractableActorBase();
		
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact(APlayerController* InstigatorPC) override;
	
	virtual void ShowInteractionPrompt() override;
	virtual void HideInteractionPrompt() override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> PromptWidgetComponent;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> InteractableMesh;
};
