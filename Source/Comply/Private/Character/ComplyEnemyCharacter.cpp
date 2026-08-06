// Copyright © 2026 Marko. All rights reserved.


#include "Character/ComplyEnemyCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/ComplyAbilitySystemComponent.h"
#include "AbilitySystem/ComplyTags.h"
#include "AbilitySystem/AttributeSets/ComplyAttributeSet.h"
#include "AbilitySystem/Data/Enemy/Stats/ComplyEnemyData.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widgets/ComplyEnemyHealthBarWidget.h"


AComplyEnemyCharacter::AComplyEnemyCharacter()
{
	ASC = CreateDefaultSubobject<UComplyAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UComplyAttributeSet>("AttributeSet");
	
	// Causes enemies to avoid each other so they don't clip through each other
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceWeight = 0.05f;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 500.f;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	
	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidgetComponent"));
	HealthWidgetComponent->SetupAttachment(RootComponent);
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HealthWidgetComponent->SetDrawAtDesiredSize(false);
	HealthWidgetComponent->SetDrawSize(FVector2D(200.f, 20.f));
}

void AComplyEnemyCharacter::OnRep_IsDead()
{
	Super::OnRep_IsDead(); // Runs ragdoll code from base class
    
	if (HealthWidgetComponent)
	{
		HealthWidgetComponent->SetVisibility(false);
	}
}

UAbilitySystemComponent* AComplyEnemyCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AComplyEnemyCharacter::InitializeHealthWidgetComponent() const
{
	if (EnemyData)
	{
		HealthWidgetComponent->SetWidgetClass(EnemyData->EnemyHealthBarWidgetClass);
		HealthWidgetComponent->InitWidget();
	}
}

// For enemies, ASC ability actor info values can just be initialized on BeginPlay
// They will have a valid controller when the game starts, as they spawn on the server
void AComplyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(GetAbilitySystemComponent())) return;
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
	
	if (HasAuthority())
	{
		InitializeAttributes();
		GiveStartupAbilities();
	}
	
	InitializeHealthWidgetComponent();

	if (UComplyEnemyHealthBarWidget* Widget = Cast<UComplyEnemyHealthBarWidget>(HealthWidgetComponent->GetWidget()))
	{
		Widget->InitializeHealthBar(GetAbilitySystemComponent());
	}
}


void AComplyEnemyCharacter::InitializeAttributes() const
{
	if (!EnemyData) return;

	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EnemyData->InitializeAttributesEffect, 1.f, ContextHandle);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ComplyTags::SetByCaller::Stats::SBC_MaxHealth, EnemyData->MaxHealth);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ComplyTags::SetByCaller::Stats::SBC_MaxArmorPenetration, EnemyData->ArmorPenetration);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AComplyEnemyCharacter::RotateHealthWidgetToPlayer()
{
	if (const APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld()))
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(
			HealthWidgetComponent->GetComponentLocation(),
			CameraLocation
		);
		HealthWidgetComponent->SetWorldRotation(LookAt);
	}
}

void AComplyEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	RotateHealthWidgetToPlayer();
}

