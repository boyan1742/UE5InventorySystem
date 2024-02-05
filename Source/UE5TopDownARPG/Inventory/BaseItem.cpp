// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"

#include "InventorySystem.h"
#include "Kismet/GameplayStatics.h"
#include "UE5TopDownARPG/UE5TopDownARPG.h"
#include "UE5TopDownARPG/UE5TopDownARPGCharacter.h"

// Sets default values
ABaseItem::ABaseItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphereComponent"));
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = SphereComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	if(MeshComponent)
	{
		MeshComponent->SetVisibility(true);
		//MeshComponent->SetWorldLocation(K2_GetActorLocation());
		//MeshComponent->SetWorldRotation(K2_GetActorRotation());
		//MeshComponent->SetWorldScale3D(GetActorScale3D());
		MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		MeshComponent->SetupAttachment(SphereComponent);
	}
	else
	{
		UE_LOG(LogUE5TopDownARPG, Error, TEXT("Mesh Component for an item is null!"));
	}
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	if(MeshComponent)
	{
		if(!StaticMesh)
		{
			static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
			if (MeshAsset.Succeeded())
			{
				StaticMesh = MeshAsset.Object;
			}
		}
		
		MeshComponent->SetStaticMesh(StaticMesh);
	}

	if(SphereComponent)
	{
		SphereComponent->SetSphereRadius(125);
		SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseItem::OnBeginOverlap);
		SphereComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ABaseItem::OnEndOverlap);
	}
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const FString& ABaseItem::GetID() const
{
	return ID;
}

const FString& ABaseItem::GetItemName() const
{
	return Name;
}

int ABaseItem::GetAmount() const
{
	return Amount;
}

UTexture2D* ABaseItem::GetIcon() const
{
	return ItemIcon;
}

void ABaseItem::HideObject()
{
	SetActorScale3D(FVector(0,0,0));
	MeshComponent->SetWorldScale3D(FVector(0,0,0));
}

void ABaseItem::ShowObject(const FVector* Position)
{
	SetActorScale3D(FVector(1,1,1));
	MeshComponent->SetWorldScale3D(FVector(1,1,1));

	if(Position)
	{
		FVector Pos(Position->X, Position->Y, GetActorLocation().Z);
		
		SetActorLocation(Pos);
		MeshComponent->SetWorldLocation(Pos);
	}
}

void ABaseItem::CopyDataFrom(const ABaseItem& Other)
{
	Name = Other.Name;
	ID = Other.ID;
	Amount = Other.Amount;
	ItemIcon = Other.ItemIcon;
}

void ABaseItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//If something else collides, we must return. We only accept Players as colliding Pawn.
	if(!IsValid(Other) || Other != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		return;
	}

	auto PlayerPawn = Cast<AUE5TopDownARPGCharacter>(Other);
	if(!PlayerPawn)
	{
		return;
	}

	if(!PlayerPawn->GetInventory().IsInventoryOpen())
	{
		PlayerPawn->ActivatePickupUI(this);
	}
	else
	{
		PlayerPawn->SetPickupItem(this);
	}
}

void ABaseItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	//If something else collides, we must return. We only accept Players as colliding Pawn.
	if(!IsValid(Other) || Other != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		return;
	}

	auto PlayerPawn = Cast<AUE5TopDownARPGCharacter>(Other);
	if(!PlayerPawn)
	{
		return;
	}

	if(PlayerPawn->IsPlayerInPickup())
	{
		PlayerPawn->DeactivatePickupUI();
	}
	else
	{
		PlayerPawn->SetPickupItem(nullptr);
	}
}
