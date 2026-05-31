// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/ComplyPlayerCharacter.h"
#include "EnforcerCharacter.generated.h"

class UCableComponent;

UCLASS()
class COMPLY_API AEnforcerCharacter : public AComplyPlayerCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnforcerCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCableComponent> GrappleCable;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> GrappleAnchorPoint;
};
