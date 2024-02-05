// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UCLASS()
class UE5TOPDOWNARPG_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	const FString& GetID() const;
	const FString& GetItemName() const;
	int GetAmount() const;
	UTexture2D* GetIcon() const;

	void AddAmount(int AdditionalAmount);

	void HideObject();
	void ShowObject(const FVector* Position = nullptr);

	void CopyDataFrom(const ABaseItem& Other);

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditDefaultsOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly)
	FString ID;

	UPROPERTY(EditDefaultsOnly)
	int Amount;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* ItemIcon;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	USphereComponent* SphereComponent;
};
