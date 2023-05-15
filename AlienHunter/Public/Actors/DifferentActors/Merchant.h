// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlienHunter/Public/Interfaces/InteractableInterface.h"
#include "AlienHunter/Public/Structs/ItemData.h"
#include "Merchant.generated.h"

class UCapsuleComponent;
class AItem;

UCLASS()
class ALIENHUNTER_API AMerchant : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
	/** The capsule component is set as a root component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComponent;

	/** Mesh is used to graphical representation of the merchant */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;
	
public:	
	// Sets default values for this actor's properties
	AMerchant();

	/**
	* Sells an item
	* @return true if the transaction was succesful  
	*/
	bool SellItem(const FItemData& ItemData,int Items = 1);

protected:
	/** The shopper the merchant is trading with */
	UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,Category = "Shop")
	ACyborg* CyborgShopper;

	/** Items for sale */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop")
	TArray<TSubclassOf<AItem>> ItemsForSale;

public:
	/** Returns Mesh */
	FORCEINLINE USkeletalMeshComponent* GetMesh() const
	{
		return Mesh;
	}


};
