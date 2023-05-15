// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlienHunter/Public/Structs/ItemData.h"
#include "EquipmentComponent.generated.h"

class AWeapon;

//It should be binded in the equipment widget blueprint graph
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponUsed, EWeaponType, WeaponType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

	/** There can be weapons only in our equipment */
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly, Category = "Equipment",meta = (AllowPrivateAccess = "true"))
	TMap<EWeaponType, AWeapon*> MappedWeaponEquipment;

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	/** Equips / adds a weapon to our equipment */
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(TSubclassOf<AItem> WeaponClass, int StackCount);

	/** Removes a weapon from our equipment */
	UFUNCTION(BlueprintCallable)
	void RemoveWeapon(EWeaponType WeaponType, bool bShouldBeDestroyed = true);

	UFUNCTION(BlueprintCallable)
	/** true if a weapon is already equiped */
	bool IsWeaponEquiped(const AWeapon* Weapon) const;
	
	/** true if a weapon of this sub class is already equiped */
	UFUNCTION(BlueprintCallable)
	bool IsWeaponClassEquiped(TSubclassOf<AItem> WeaponClass) const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWeapon* GetWeaponOfType(EWeaponType WeaponType) const
	{
		return MappedWeaponEquipment.FindRef(WeaponType);
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsEmpty() const
	{
		return MappedWeaponEquipment.IsEmpty();
	}

public:
	UPROPERTY(BlueprintAssignable)
	FOnWeaponUsed OnWeaponUsed;

	UFUNCTION(BlueprintCallable)
	void SetCurrentUsedWeaponType(EWeaponType WeaponType);

public:
	/**
	* Type of the weapon that owner would like to use, but it doesn't have to be in his equipment
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Equipment")
	EWeaponType DesiredWeaponType = EWeaponType::Unknown;

protected:
	AWeapon* CreateWeapon(TSubclassOf<AItem> WeaponClass, int StackCount);

protected:
	/** Called when equipment has been changed */
	UPROPERTY(BlueprintAssignable,Category="Equipment")
	FOnEquipmentChanged OnEquipmentChanged;

	/** The type of the weapon that the cyborg is holding in his hand */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Equipment")
	EWeaponType CurrentUsedWeaponType;

public:	
	/** Returns character mapped equipment */
	FORCEINLINE const TMap<EWeaponType, AWeapon*>& GetMappedWeaponEquipment() const 
	{
		return MappedWeaponEquipment; 
	}
		
};
