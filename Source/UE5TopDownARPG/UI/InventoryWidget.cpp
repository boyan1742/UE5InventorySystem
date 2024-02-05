// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"

#include "UE5TopDownARPG/UE5TopDownARPG.h"

void UInventoryWidget::SyncItems(const TArray<ABaseItem*>& Items)
{
	int i = 0;
	for (auto& Item : Items)
	{
		if (!Item)
		{
			continue;
		}

		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Item name: %s"), *Item->GetItemName());

		auto ItemWidget = GetSlot(i);
		if (ItemWidget)
		{
			UE_LOG(LogUE5TopDownARPG, Log, TEXT("Updating slot #%d."), i);
			ItemWidget->UpdateLook(Item);
		}

		i++;
		if (i >= MAX_ITEMS)
		{
			break;
		}
	}

	for (; i < MAX_ITEMS; i++)
	{
		auto ItemWidget = GetSlot(i);
		if (ItemWidget)
		{
			ItemWidget->UpdateLook(nullptr);
		}
	}
}

void UInventoryWidget::CheckForDuplicateSelected()
{
	FDateTime MaxTime = FDateTime::MinValue();
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		UInventoryItemWidget* ItemWidget = GetSlot(i);
		if (ItemWidget && ItemWidget->IsItemSelected() &&
			MaxTime < ItemWidget->GetLastTimeSelected())
		{
			MaxTime = ItemWidget->GetLastTimeSelected();
		}
	}

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		UInventoryItemWidget* ItemWidget = GetSlot(i);
		if (ItemWidget && ItemWidget->IsItemSelected())
		{
			if (MaxTime != ItemWidget->GetLastTimeSelected())
			{
				ItemWidget->Deselect();
			}
			else
			{
				SelectedSlot = i;
			}
		}
	}
}

int UInventoryWidget::GetSelectedSlot()
{
	return SelectedSlot;
}

UInventoryItemWidget* UInventoryWidget::GetSlot(int Index)
{
	switch (Index)
	{
	case 0:
		return Slot0;
	case 1:
		return Slot1;
	case 2:
		return Slot2;
	case 3:
		return Slot3;
	case 4:
		return Slot4;
	case 5:
		return Slot5;
	case 6:
		return Slot6;
	case 7:
		return Slot7;
	default:
		return nullptr;
	}
}
