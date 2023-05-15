// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "Structs/ItemData.h"
#include "Item.generated.h"

class UBoxComponent;
class ACyborg;
class AAlienHunterCharacter;

UCLASS()
class ALIENHUNTER_API AItem : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	/** Mesh is used to graphical representation of the item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

public:
	// Sets default values for this actor's properties
	AItem();

	/** Function for interacting with player, if an item is neither in player inventory nor equipment */
	bool Interact(ACyborg* Character);
	virtual bool Interact_Implementation(ACyborg* Character) override;

	/**
	* Funtion to start using this item
	*/
	virtual void Use(AAlienHunterCharacter* Character) {};

protected:
	virtual void BeginPlay() override;

	/** Creates Text info about this item */
	virtual void CreateItemTextInfo();

	/** PostLoad override. */
	virtual void PostLoad() override;

public:
	/** Item data is used to create an item inventory / equipment widget*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemData ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* PickUpSound;

public:
	/** Returns Mesh */
	FORCEINLINE USkeletalMeshComponent* GetMesh() const 
	{ 
		return Mesh; 
	}

};