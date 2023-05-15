// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Components/EquipmentComponent.h"
#include "Actors/Characters/Cyborg.h"
#include "Actors/Items/Item.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UInventoryComponent::AddItem(const FItemData& NewItemData, int Items, bool bUpdateGameInstance)
{
	if (!Items)
	{
		return false;
	}
	bool bWasItemAdded = false;
	FItemData* const ItemData = InventoryItemData.FindByPredicate
	(
		[&NewItemData](const FItemData& ItemData)
		{
			return NewItemData == ItemData;
		}
	);

	if (!ItemData)
	{
		InventoryItemData.Add(NewItemData);
		FItemData& LastItemData = InventoryItemData.Last();
		LastItemData.StackCount = Items;
		bWasItemAdded = true;
		OnInventoryChanged.Broadcast(LastItemData);
	}
	else if (!ItemData->bCanBeOnlyOne)
	{
		ItemData->StackCount += Items;
		bWasItemAdded = true;
		OnInventoryChanged.Broadcast(*ItemData);
		if(OwnerEquipment->IsWeaponClassEquiped(ItemData->ItemClass))
		{
			OwnerEquipment->EquipWeapon(ItemData->ItemClass, ItemData->StackCount);
		}
	}

	return bWasItemAdded;
}

void UInventoryComponent::RemoveItem(TSubclassOf<AItem> ItemToBeRemoved, int AmmountToBeRemoved)
{
	uint32 ItemIndex = 0;
	for (FItemData& ItemData : InventoryItemData)
	{
		if (ItemData.ItemClass == ItemToBeRemoved)
		{
			if (ItemData.bCanBeOnlyOne)
			{
				return;
			}
			ItemData.StackCount -= AmmountToBeRemoved;
			if (OwnerEquipment && OwnerEquipment->IsWeaponClassEquiped(ItemToBeRemoved))
			{
				OwnerEquipment->RemoveWeapon(ItemData.WeaponType, false);
			}
			OnInventoryChanged.Broadcast(ItemData);
			const bool bShouldBeRemoved = ItemData.StackCount <= 0;
			if (bShouldBeRemoved)
			{
				InventoryItemData.RemoveAt(ItemIndex);
			}
			break;
		}
		++ItemIndex;
	}
}

bool UInventoryComponent::DoesContainItemType(TSubclassOf<AItem> ItemClass) const
{
	const FItemData* const ItemData = InventoryItemData.FindByPredicate
	(
		[&ItemClass](const FItemData& ItemData) {return ItemData == ItemClass; }
	);
	return ItemData != nullptr;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (ACyborg* const Cyborg = Cast<ACyborg>(GetOwner()))
	{
		OwnerEquipment = Cyborg->GetEquipmentComponent();
	}
}
