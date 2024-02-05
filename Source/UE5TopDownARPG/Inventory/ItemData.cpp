#include "ItemData.h"

FItemData::FItemData(): Amount(0)
{
}

FItemData::FItemData(const FString& ID, const FString& Name, int Amount, const FTexture& Icon)
{
	this->ID = ID;
	this->Name = Name;
	this->Amount = Amount;
	ItemIcon = Icon;
}

FItemData::FItemData(const FItemData& Copy)
{
	CopyDataFrom(Copy);
}

const FString& FItemData::GetID() const
{
	return ID;
}

const FString& FItemData::GetItemName() const
{
	return Name;
}

int FItemData::GetAmount() const
{
	return Amount;
}

const FTexture& FItemData::GetIcon() const
{
	return ItemIcon;
}

void FItemData::CopyDataFrom(const FItemData& Other)
{
	Name = Other.Name;
	ID = Other.ID;
	Amount = Other.Amount;
	ItemIcon = Other.ItemIcon;
}
