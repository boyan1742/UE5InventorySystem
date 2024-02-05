// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5TOPDOWNARPG_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateLook(class ABaseItem* Item);
	bool IsItemSelected() const;
	void Deselect();
	const FDateTime& GetLastTimeSelected(); 

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta=(BindWidget))
	UImage* ItemIconImage;
	UPROPERTY(meta=(BindWidget))
	UBorder* ItemIconBorder;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemAmountText;

	bool SelectedByUser = false;
	FDateTime LastTimeSelected = FDateTime::Now();
};
