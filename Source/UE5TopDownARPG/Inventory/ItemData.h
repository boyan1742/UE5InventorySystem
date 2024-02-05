#pragma once

class FItemData
{
public:
	FItemData();
	FItemData(const FString& ID, const FString& Name, int Amount, const FTexture& Icon);
	FItemData(const FItemData& Copy);
	
	const FString& GetID() const;
	const FString& GetItemName() const;
	int GetAmount() const;
	const FTexture& GetIcon() const;

	void CopyDataFrom(const FItemData& Other);

private:
	FString Name;
	FString ID;
	int Amount;
	FTexture ItemIcon;
};
