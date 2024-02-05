// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TopDownARPGCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Inventory/InventorySystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Abilities/BaseAbility.h"
#include "UE5TopDownARPGGameMode.h"
#include "UE5TopDownARPGPlayerController.h"
#include "UE5TopDownARPG.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HealthbarWidget.h"
#include "Net/UnrealNetwork.h"
#include "UI/InventoryPickupItemWidget.h"

AUE5TopDownARPGCharacter::AUE5TopDownARPGCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetCastShadow(false);
	WidgetComponent->SetReceivesDecals(false);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetupAttachment(RootComponent);

	InventorySystem = CreateDefaultSubobject<UInventorySystem>(TEXT("Inventory System"));

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	OnTakeAnyDamage.AddDynamic(this, &AUE5TopDownARPGCharacter::TakeAnyDamage);
}

void AUE5TopDownARPGCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (IsValid(WidgetComponent->GetWidgetClass()))
	{
		WidgetComponent->InitWidget();
		HealthbarWidget = Cast<UHealthbarWidget>(WidgetComponent->GetUserWidgetObject());
	}
}

void AUE5TopDownARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilityTemplate != nullptr)
	{
		AbilityInstance = NewObject<UBaseAbility>(this, AbilityTemplate);
	}
	if (IsValid(HealthbarWidget))
	{
		float HealthPercent = Health / MaxHealth;
		HealthbarWidget->SetPercent(HealthPercent);
	}

	InventorySystem = NewObject<UInventorySystem>(this, TEXT("Inventory System"));
	if (InventorySystem)
	{
		InventorySystem->SetInventoryWidgetClass(InventoryWidgetClass);
		InventorySystem->Init();
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController)
	{
		InventoryPickupItemWidget = Cast<UInventoryPickupItemWidget>(
			CreateWidget(PlayerController, InventoryPickupItemWidgetClass));
		if (InventoryPickupItemWidget)
		{
			InventoryPickupItemWidget->AddToViewport();
			InventoryPickupItemWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AUE5TopDownARPGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(InventorySystem)
	{
		InventorySystem->OnTick();
	}
	
	/*
	FHitResult HitResult;
	FVector TraceStartLocation = GetActorLocation();
	FVector TraceEndLocation = GetActorLocation() + GetActorForwardVector() * 300.0f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStartLocation, TraceEndLocation, ECollisionChannel::ECC_WorldDynamic, Params))
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("TraceHit %s %s"), *HitResult.GetActor()->GetName(), *HitResult.GetComponent()->GetName());
	}
	*/
}

void AUE5TopDownARPGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUE5TopDownARPGCharacter, Health);
	DOREPLIFETIME(AUE5TopDownARPGCharacter, MaxHealth);
}

bool AUE5TopDownARPGCharacter::ActivateAbility(FVector Location)
{
	if (IsValid(AbilityInstance))
	{
		return AbilityInstance->Activate(Location);
	}
	return false;
}

UInventorySystem& AUE5TopDownARPGCharacter::GetInventory() const
{
	return *InventorySystem;
}

bool AUE5TopDownARPGCharacter::IsPlayerInPickup() const
{
	return InventoryPickupItemWidget && InventoryPickupItemWidget->GetVisibility() == ESlateVisibility::Visible;
}

ABaseItem* AUE5TopDownARPGCharacter::GetPickupItem() const
{
	return Cast<ABaseItem>(PickupItem);
}

void AUE5TopDownARPGCharacter::ActivatePickupUI(ABaseItem* Item)
{
	PickupItem = Item;
	
	if (InventoryPickupItemWidget && InventoryPickupItemWidget->GetVisibility() != ESlateVisibility::Visible)
	{
		InventoryPickupItemWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AUE5TopDownARPGCharacter::DeactivatePickupUI()
{
	PickupItem = nullptr;
	
	if (InventoryPickupItemWidget && InventoryPickupItemWidget->GetVisibility() != ESlateVisibility::Collapsed)
	{
		InventoryPickupItemWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AUE5TopDownARPGCharacter::SetPickupItem(ABaseItem* Item)
{
	PickupItem = Item;
}

void AUE5TopDownARPGCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                             AController* InstigateBy, AActor* DamageCauser)
{
	Health -= Damage;
	OnRep_SetHealth(Health + Damage);
	UE_LOG(LogUE5TopDownARPG, Log, TEXT("Health %f"), Health);
	if (IsValid(HealthbarWidget))
	{
		float HealthPercent = Health / MaxHealth;
		HealthbarWidget->SetPercent(HealthPercent);
	}
	if (Health <= 0.0f)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(DeathHandle) == false)
		{
			GetWorld()->GetTimerManager().SetTimer(DeathHandle, this, &AUE5TopDownARPGCharacter::Death, DeathDelay);
		}
	}
}

void AUE5TopDownARPGCharacter::OnRep_SetHealth(float OldHealth)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health %f"), Health));
	}
}

void AUE5TopDownARPGCharacter::Death()
{
	UE_LOG(LogUE5TopDownARPG, Log, TEXT("Death"));
	AUE5TopDownARPGGameMode* GameMode = Cast<AUE5TopDownARPGGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(GameMode))
	{
		GameMode->EndGame(false);
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();
	if (FMath::RandBool())
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor(AfterDeathSpawnClass, &Location, &Rotation, SpawnParameters);
	}

	GetWorld()->GetTimerManager().ClearTimer(DeathHandle);
	AUE5TopDownARPGPlayerController* PlayerController = Cast<AUE5TopDownARPGPlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		PlayerController->OnPlayerDied();
	}
	Destroy();
}
