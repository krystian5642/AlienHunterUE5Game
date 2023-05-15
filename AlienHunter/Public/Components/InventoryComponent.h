// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlienHunter/Public/Structs/ItemData.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChanged, FItemData, NewItemData);

class UEquipmentComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	/** Adds an item to the inventory item data*/
	UFUNCTION(BlueprintCallable)
	bool AddItem(const FItemData& NewItemData, int Items = 1, bool bUpdateGameInstance = true);

	/** Removes item from the inventory item data */
	UFUNCTION(BlueprintCallable)
	void RemoveItem(TSubclassOf<AItem> ItemToBeRemoved, int AmmountToBeRemoved = 1);

	UFUNCTION(BlueprintCallable)
	bool DoesContainItemType(TSubclassOf<AItem> ItemClass) const;

public:
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;

protected:
	virtual void BeginPlay() override;

protected:
	/** Data of all items kept in the inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemData> InventoryItemData;

	UPROPERTY()
	UEquipmentComponent* OwnerEquipment;
		
};
