// Copyright © 2026 Marko. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldDome.generated.h"

class UShieldUtilityData;
class UNiagaraComponent;
class UAbilitySystemComponent;
class UGameplayAbility;
class USphereComponent;
class UNavModifierComponent;

UCLASS()
class COMPLY_API AShieldDome : public AActor
{
	GENERATED_BODY()

public:
	AShieldDome();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	TObjectPtr<UShieldUtilityData> ShieldData;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldStaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShieldObjectStaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category = "Components")
	TObjectPtr<USphereComponent> SphereComp;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ShieldHumNiagaraComponent;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> HumAudioComponent;
};
