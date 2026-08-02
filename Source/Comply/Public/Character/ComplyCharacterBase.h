// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ComplyCharacter.h"
#include "GameplayTagContainer.h"
#include "Interface/CombatantInterface.h"
#include "ComplyCharacterBase.generated.h"


class UGameplayAbility;
class UGameplayEffect;

// Maps abilities to input tags, set when adding the struct to startup abilities in blueprint
USTRUCT()
struct FAbilitySet
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AbilityTypeTag;
};

UCLASS(Abstract)
class COMPLY_API AComplyCharacterBase : public AComplyCharacter, public IAbilitySystemInterface, public ICombatantInterface
{
	GENERATED_BODY()

public:
	AComplyCharacterBase();
	
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// This function is called when a new character class is being equipped
	// This ensures the old class abilities are cleared, and the new character's abilities can be used
	void ClearStartupAbilities();
	
	virtual void Die(AActor* DeadActor) override;
	
	virtual void HandleHit(const AActor* HitActor) override;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Applies the gameplay effect that initializes attribute values 
	// The function will be implemented here and called on child classes after we know the ASC is valid (server only)
	virtual void InitializeAttributes() const;
	
	// Gives each ability in the StartupAbilities TArray to the character's ASC, and sets its input tag (server only)
	void GiveStartupAbilities();
	
	void ActivateInitialAbility() const;
	
	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead = false;
	
	UFUNCTION()
	void OnRep_IsDead();

public:	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeAttributesEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitializeWeaponAttributesEffect;
	
	virtual void Tick(float DeltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TArray<FAbilitySet> StartupAbilities;
	
	UPROPERTY(EditAnywhere, Category = "Animations")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitReact();
};
