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
UCLASS()
class COMPLY_API UBTTask_ActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_ActivateAbility();
	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> AbilityToActivate;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag AbilityEventTag;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActorKey;
	
private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);

	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
	FGameplayAbilitySpecHandle ActiveSpecHandle;
	FDelegateHandle AbilityEndedDelegateHandle;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
