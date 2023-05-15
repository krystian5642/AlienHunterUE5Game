#pragma once

#include "ItemData.generated.h"

class AItem;
class UTexture2D;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Unknown,
	Medical,
	TeleportKey,
	Weapon
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unknown,
	Rifle,
	Pistol,
	Grenade,
	Knife
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()
	
	FItemData()
	: ItemTexture(nullptr)
	, ItemType(EItemType::Unknown)
	, WeaponType(EWeaponType::Unknown)
	, bCanBeOnlyOne(true)
	, StackCount(1)
	, ItemCost(0)
	, ItemName(TEXT(""))
	, ItemTextInfo()
	{
	}

	/** ItemClass is needed to spawn it properly when the player needs this item spawned */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItem> ItemClass;

	/** 2d texture is needed to display it in the player inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* ItemTexture;

	/** Item type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemType ItemType;

	/** Edit only if item is of type weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (EditCondition = "ItemType == EItemType::Weapon"))
	EWeaponType WeaponType;

	/** if true item can be only one in the inventory*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanBeOnlyOne;

	/** Actual number of items of given types in the inventory */
	UPROPERTY(BlueprintReadWrite, meta = (ClampMin = 0 , EditCondition = "!bCanBeOnlyOne"))
	int StackCount;

	/** Actual cost of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ClampMin = 0.0f))
	float ItemCost;

	/** All items of the same type share the same name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(BlueprintReadOnly)
	FString ItemTextInfo;

	FORCEINLINE bool operator==(const FItemData& ItemData) const
	{
		return ItemClass == ItemData.ItemClass;
	}

	FORCEINLINE bool operator==(const TSubclassOf<AItem>& ItemDataClass) const
	{
		return ItemClass == ItemDataClass;
	}
};