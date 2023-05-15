// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Items/Item.h"
#include "Components/BoxComponent.h"
#include "Actors/Characters/Cyborg.h"
#include "Components/InventoryComponent.h"
#include "Components/QuestLogComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(Mesh);

	ItemData.ItemClass = GetClass();

	static ConstructorHelpers::FObjectFinder<USoundBase> PickUpSoundObj(TEXT("/Game/Sounds/PickUpSound_Cue"));
	if (PickUpSoundObj.Succeeded())
	{
		PickUpSound = PickUpSoundObj.Object;
	}
}

bool AItem::Interact_Implementation(ACyborg* Character)
{
	if (!IsValid(Character))
	{
		return false;
	}

	bool bWasItemAdded = false;
	bWasItemAdded = Character->GetInventoryComponent()->AddItem(ItemData);
	
	if (bWasItemAdded)
	{
		if (PickUpSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, PickUpSound, GetActorLocation());
		}
		Character->OnItemCollected.Broadcast(GetClass());
		Destroy();
	}
	return bWasItemAdded;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::CreateItemTextInfo()
{
	ItemData.ItemTextInfo.Reset();

	ItemData.ItemTextInfo.Append("Name : " + ItemData.ItemName.ToString() + "\n");

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EItemType"), true);
	FString EnumString = EnumPtr ? EnumPtr->GetValueAsString(ItemData.ItemType) : FString("Invalid");
	EnumString = EnumString.RightChop(FString("EItemType::").Len());

	ItemData.ItemTextInfo.Append("Item Type : " + EnumString);
}

void AItem::PostLoad()
{
	Super::PostLoad();
	CreateItemTextInfo();
}
