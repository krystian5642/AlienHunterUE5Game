// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/Merchant.h"
#include "Actors/Characters/Cyborg.h"
#include "Actors/Items/Item.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMerchant::AMerchant()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

bool AMerchant::SellItem(const FItemData& ItemData,int Items)
{
	if (!IsValid(CyborgShopper) || Items<=0)
	{
		return false;
	}

	TSubclassOf<AItem>* const ItemForSaleClassPtr = ItemsForSale.FindByPredicate
	(
		[&ItemData](const TSubclassOf<AItem>& ItemForSaleClass)
		{
			return ItemData == ItemForSaleClass;
		}
	);

	bool bWasTransactionSuccessful = false;
	if (ItemForSaleClassPtr)
	{
		if (AItem* const DefaultItem = ItemForSaleClassPtr->GetDefaultObject())
		{
			const int Price = DefaultItem->ItemData.ItemCost * Items;
			bWasTransactionSuccessful = CyborgShopper->Pay(Price);
		}
	}
	return bWasTransactionSuccessful;
}