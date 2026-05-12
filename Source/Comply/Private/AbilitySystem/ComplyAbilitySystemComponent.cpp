// Copyright © 2026 Marko. All rights reserved.


#include "AbilitySystem/ComplyAbilitySystemComponent.h"


UComplyAbilitySystemComponent::UComplyAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UComplyAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UComplyAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

