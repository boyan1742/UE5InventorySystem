// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemWidget.h"
#include "Blueprint/UserWidget.h"
#include "UE5TopDownARPG/Inventory/BaseItem.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5TOPDOWNARPG_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SyncItems(const TArray<ABaseItem*>& Items);
	void CheckForDuplicateSelected();
	int GetSelectedSlot();

private:
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot0;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot1;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot2;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot3;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot4;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot5;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot6;
	UPROPERTY(meta=(BindWidget))
	UInventoryItemWidget* Slot7;

	const int MAX_ITEMS = 8;

	int SelectedSlot = -1;

	UInventoryItemWidget* GetSlot(int Index);
};
