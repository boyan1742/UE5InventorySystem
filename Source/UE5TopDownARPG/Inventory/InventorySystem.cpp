// Fill out your copyright notice in the Description page of Project Settings.

#include "InventorySystem.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "UE5TopDownARPG/UE5TopDownARPG.h"

UInventorySystem::UInventorySystem()
{
	IsInited = false;
}

void UInventorySystem::Init()
{
	if (IsInited)
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PlayerController))
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(PlayerController, InventoryWidgetClass);
		if (IsValid(InventoryWidget))
		{
			InventoryWidget->AddToViewport();
			//InventoryWidget->AddToPlayerScreen();
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
			IsInited = true;
			return;
		}
	}

	IsInited = false;
}

void UInventorySystem::OnTick()
{
	if (InventoryWidget)
	{
		InventoryWidget->CheckForDuplicateSelected();
	}
}

void UInventorySystem::OpenInventory()
{
	if (!IsInited)
	{
		return;
	}

	if (IsValid(InventoryWidget))
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Sync Items"));
		InventoryWidget->SyncItems(Items);
		InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventorySystem::CloseInventory()
{
	if (!IsInited)
	{
		return;
	}

	if (IsValid(InventoryWidget))
	{
		/*UE_LOG(LogUE5TopDownARPG, Log, TEXT("Sync Items"));
		InventoryWidget->SyncItems(Items);*/
		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

bool UInventorySystem::IsInventoryOpen() const
{
	return IsValid(InventoryWidget) && InventoryWidget->GetVisibility() == ESlateVisibility::Visible;
}

void UInventorySystem::AddItemOnGround(ABaseItem* Item)
{
	if (!IsInited || !IsValid(Item))
	{
		return;
	}

	Items.Add(Item);
	//GetWorld()->DestroyActor(Item);
	Item->HideObject();
	UE_LOG(LogUE5TopDownARPG, Log, TEXT("Added item %s | %s"), *Item->GetItemName(), *Item->GetID());
	if (IsValid(InventoryWidget) && InventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Sync Items"));
		InventoryWidget->SyncItems(Items);
	}
}

void UInventorySystem::AddCreatedItem(ABaseItem* Item)
{
	if (!IsInited || !Item)
	{
		return;
	}

	Items.Add(Item);
	Item->HideObject();
	if (IsValid(InventoryWidget) && InventoryWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Sync Items"));
		InventoryWidget->SyncItems(Items);
	}
}

void UInventorySystem::DropItem(const FString& ItemID)
{
	if (!IsInited)
	{
		return;
	}

	int Index = 0;
	bool HasAnItemBeenDropped = false;
	for (auto& Item : Items)
	{
		if (Item->GetID().Compare(ItemID, ESearchCase::IgnoreCase) == 0)
		{
			//We found the item we searched for. So we drop it in the feet of our character.
			const FVector Position = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetCharacter()->GetActorLocation();

			/*AActor* SpawnedItem = GetWorld()->SpawnActor(TSubclassOf<ABaseItem>(), &Position);
			if(IsValid(SpawnedItem))
			{
				ABaseItem* DroppedItem = Cast<ABaseItem>(SpawnedItem);
				DroppedItem->CopyDataFrom(*Item);
			}*/

			Item->ShowObject(&Position);

			HasAnItemBeenDropped = true;
			break;
		}
		Index++;
	}

	if (HasAnItemBeenDropped)
	{
		Items.RemoveAt(Index);
		if (IsValid(InventoryWidget) && InventoryWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("Sync Items"));
			InventoryWidget->SyncItems(Items);
		}
	}
}

TOptional<const FString> UInventorySystem::GetSelectedItemID()
{
	if (!InventoryWidget)
	{
		return TOptional<const FString>();
	}

	int SelectedSlot = InventoryWidget->GetSelectedSlot();
	if(SelectedSlot != -1)
	{
		if(SelectedSlot >= Items.Num())
			return TOptional<const FString>();
		
		ABaseItem* Item = Items[SelectedSlot];
		if(Item)
		{
			return TOptional<const FString>(Item->GetID());
		}
	}

	return TOptional<const FString>();
}

void UInventorySystem::SetInventoryWidgetClass(TSubclassOf<UUserWidget> Class)
{
	InventoryWidgetClass = Class;
}
