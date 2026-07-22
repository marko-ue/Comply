// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActorBase.h"
#include "MissionSelect.generated.h"

UCLASS()
class COMPLY_API AMissionSelect : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	AMissionSelect();
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact(APlayerController* InstigatorPC) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MissionSelectWidgetClass;
};
