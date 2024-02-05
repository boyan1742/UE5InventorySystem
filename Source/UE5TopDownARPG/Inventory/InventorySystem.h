// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "UE5TopDownARPG/UI/InventoryWidget.h"
#include "InventorySystem.generated.h"

UCLASS(Blueprintable)
class UE5TOPDOWNARPG_API UInventorySystem : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UInventorySystem();

	void Init();
	void OnTick();
	
	void OpenInventory();
	void CloseInventory();

	bool IsInventoryOpen() const;

	void AddItem(ABaseItem* Item);
	ABaseItem* GetItem(const FString& ItemID);
	void DropItem(const FString& ItemID);
	void UseItem(const FString& ItemID);

	TOptional<const FString> GetSelectedItemID();

	void SetInventoryWidgetClass(TSubclassOf<class UUserWidget> Class);

private:
	UPROPERTY(EditDefaultsOnly)
	UInventoryWidget* InventoryWidget;

	bool IsInited;
	TArray<ABaseItem*> Items;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> InventoryWidgetClass;
};
