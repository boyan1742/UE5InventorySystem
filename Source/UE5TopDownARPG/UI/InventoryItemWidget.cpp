// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"

#include "UE5TopDownARPG/UE5TopDownARPG.h"
#include "UE5TopDownARPG/Inventory/BaseItem.h"

void UInventoryItemWidget::UpdateLook(ABaseItem* Item)
{
	if(!Item)
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Tried to update the image texture with a null item."));
		ItemIconImage->SetBrushFromTexture(nullptr, true);
		ItemAmountText->SetText(FText::FromString("0"));
		ItemAmountText->SetColorAndOpacity(FSlateColor(FColor(1,1,1, 0)));
		return;
	}

	if(!Item->GetIcon())
	{
		UE_LOG(LogUE5TopDownARPG, Log, TEXT("Tried to update the image texture with an item with null texture."));
		ItemIconImage->SetBrushFromTexture(nullptr, true);
	}
	else
	{
		ItemIconImage->SetBrushFromTexture(Item->GetIcon(), true);
	}

	if(Item->GetAmount() > 0)
	{
		ItemAmountText->SetText(FText::FromString(FString::FromInt(Item->GetAmount())));
		ItemAmountText->SetColorAndOpacity(FSlateColor(FColor(0,0,0)));
	}
}

bool UInventoryItemWidget::IsItemSelected() const
{
	return SelectedByUser;
}

void UInventoryItemWidget::Deselect()
{
	SelectedByUser = false;
	ItemIconBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f));
}

const FDateTime& UInventoryItemWidget::GetLastTimeSelected()
{
	return LastTimeSelected;
}

FReply UInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		SelectedByUser = true;
		LastTimeSelected = FDateTime::Now();
		ItemIconBorder->SetBrushColor(FLinearColor(1.0f, 0.0f, 0.0f));
	}

	return FReply::Handled();
}
