// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5TopDownARPGPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "Inventory/InventorySystem.h"
#include "NiagaraFunctionLibrary.h"
#include "UE5TopDownARPGCharacter.h"
#include "UE5TopDownARPGHUD.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UE5TopDownARPG.h"

AUE5TopDownARPGPlayerController::AUE5TopDownARPGPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AUE5TopDownARPGPlayerController::OnPlayerDied()
{
	AUE5TopDownARPGHUD* HUD = Cast<AUE5TopDownARPGHUD>(GetHUD());
	if (IsValid(HUD))
	{
		HUD->ShowEndGameScreen();
	}
}

void AUE5TopDownARPGPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AUE5TopDownARPGPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AUE5TopDownARPGPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AUE5TopDownARPGPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AUE5TopDownARPGPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AUE5TopDownARPGPlayerController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(ActivateAbilityAction, ETriggerEvent::Started, this, &AUE5TopDownARPGPlayerController::OnActivateAbilityStarted);

		//Setup Inventory Keybinds
		EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AUE5TopDownARPGPlayerController::OnInventoryOpenStarted);
		EnhancedInputComponent->BindAction(PickUpDropItemAction, ETriggerEvent::Started, this, &AUE5TopDownARPGPlayerController::OnInventoryItemDropPickupStarted);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &AUE5TopDownARPGPlayerController::OnInventoryItemUseStarted);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AUE5TopDownARPGPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AUE5TopDownARPGPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AUE5TopDownARPGPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AUE5TopDownARPGPlayerController::OnTouchReleased);
	}
}

void AUE5TopDownARPGPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AUE5TopDownARPGPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AUE5TopDownARPGPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AUE5TopDownARPGPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AUE5TopDownARPGPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AUE5TopDownARPGPlayerController::OnActivateAbilityStarted()
{
	UE_LOG(LogUE5TopDownARPG, Log, TEXT("OnActivateAbilityStarted"));

	AUE5TopDownARPGCharacter* ARPGCharacter = Cast<AUE5TopDownARPGCharacter>(GetPawn());
	if (IsValid(ARPGCharacter))
	{
		FHitResult Hit;
		bool bHitSuccessful = false;
		if (bIsTouch)
		{
			bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
		}
		else
		{
			bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
		}

		// If we hit a surface, cache the location
		if (bHitSuccessful)
		{
			ARPGCharacter->ActivateAbility(Hit.Location);
		}
	}
}

void AUE5TopDownARPGPlayerController::OnInventoryOpenStarted()
{
	AUE5TopDownARPGCharacter* PlayerCharacter = Cast<AUE5TopDownARPGCharacter>(GetCharacter());
	if(IsValid(PlayerCharacter))
	{
		if(PlayerCharacter->GetInventory().IsInventoryOpen())
		{
			PlayerCharacter->GetInventory().CloseInventory();

			if(PlayerCharacter->GetPickupItem())
			{
				PlayerCharacter->ActivatePickupUI(PlayerCharacter->GetPickupItem());
			}
		}
		else
		{
			PlayerCharacter->GetInventory().OpenInventory();

			if(PlayerCharacter->IsPlayerInPickup())
			{
				ABaseItem* Item = PlayerCharacter->GetPickupItem();
				PlayerCharacter->DeactivatePickupUI();
				PlayerCharacter->SetPickupItem(Item);
			}
		}
	}
}

void AUE5TopDownARPGPlayerController::OnInventoryItemDropPickupStarted()
{
	AUE5TopDownARPGCharacter* PlayerCharacter = Cast<AUE5TopDownARPGCharacter>(GetCharacter());
	if(!PlayerCharacter)
	{
		return;
	}

	if(PlayerCharacter->GetInventory().IsInventoryOpen())
	{
		TOptional<const FString> SelectedItemID = PlayerCharacter->GetInventory().GetSelectedItemID();
		if(SelectedItemID.IsSet())
		{
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("Attempting to drop item: %s"), *SelectedItemID.GetValue())
			ABaseItem* Item = PlayerCharacter->GetInventory().GetItem(SelectedItemID.GetValue()); 
			PlayerCharacter->GetInventory().DropItem(SelectedItemID.GetValue());
			PlayerCharacter->GetInventory().CloseInventory();

			UE_LOG(LogUE5TopDownARPG, Log, TEXT("Pickup Item: %p"), PlayerCharacter->GetPickupItem())
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("Item: %p"), Item)
			
			if(Item && !PlayerCharacter->GetPickupItem())
			{
				UE_LOG(LogUE5TopDownARPG, Log, TEXT("Activating UI for dropped item: %s"), *SelectedItemID.GetValue())
				PlayerCharacter->ActivatePickupUI(Item);
			}
			else if(PlayerCharacter->GetPickupItem())
			{
				UE_LOG(LogUE5TopDownARPG, Log, TEXT("Activating UI for overlapping item: %s"), *SelectedItemID.GetValue())
				PlayerCharacter->ActivatePickupUI(PlayerCharacter->GetPickupItem());
			}
		}
	}
	else if(PlayerCharacter->IsPlayerInPickup())
	{
		//We must pickup the item.
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Pickup Item: %p"), PlayerCharacter->GetPickupItem())
		PlayerCharacter->GetInventory().AddItem(PlayerCharacter->GetPickupItem());
		PlayerCharacter->SetPickupItem(nullptr);
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Pickup Item: %p"), PlayerCharacter->GetPickupItem())
	}
}

void AUE5TopDownARPGPlayerController::OnInventoryItemUseStarted()
{
	AUE5TopDownARPGCharacter* PlayerCharacter = Cast<AUE5TopDownARPGCharacter>(GetCharacter());
	if(!PlayerCharacter)
	{
		return;
	}

	if(PlayerCharacter->GetInventory().IsInventoryOpen())
	{
		TOptional<const FString> SelectedItemID = PlayerCharacter->GetInventory().GetSelectedItemID();
		if(SelectedItemID.IsSet())
		{
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("Attempting to drop item: %s"), *SelectedItemID.GetValue())
			PlayerCharacter->GetInventory().UseItem(SelectedItemID.GetValue());
			PlayerCharacter->GetInventory().CloseInventory();
		}
	}
}
