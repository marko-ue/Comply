// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "Character/ComplyPlayerCharacter.h"

#include "EnforcerCharacter.generated.h"

class UCableComponent;

/**
 * Class inherited to add a cable and scene component only for the Enforcer that uses a grappling hook.
 */
UCLASS()
class COMPLY_API AEnforcerCharacter : public AComplyPlayerCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnforcerCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCableComponent> GrappleCable;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> GrappleAnchorPoint;
};
