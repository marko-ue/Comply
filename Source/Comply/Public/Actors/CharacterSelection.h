// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Player/InteractableInterface.h"
#include "CharacterSelection.generated.h"

UCLASS()
class COMPLY_API ACharacterSelection : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ACharacterSelection();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact(APlayerController* InstigatorPC) override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> InteractableMesh;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> CharacterSelectionWidgetClass;
	
};
