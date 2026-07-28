// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ActivateAbility.generated.h"

class UAbilitySystemComponent;
struct FAbilityEndedData;
class UGameplayAbility;
/**
 * 
 */

// Per-robot state stored in NodeMemory, one block allocated per AI instance running this task
// This is needed when there are multiple enemies at once, since member variables are shared across all AI running the same BT asset
struct FBTTask_ActivateAbilityMemory
{
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	TWeakObjectPtr<UGameplayAbility> CachedAbilityInstance;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	FGameplayAbilitySpecHandle ActiveSpecHandle;
	FDelegateHandle AbilityEndedDelegateHandle;
};

UCLASS()
class COMPLY_API UBTTask_ActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ActivateAbility();
	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual uint16 GetInstanceMemorySize() const override;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> AbilityToActivate;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag AbilityEventTag;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
