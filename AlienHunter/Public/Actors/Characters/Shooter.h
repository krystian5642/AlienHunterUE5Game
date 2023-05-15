// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlienHunter/AlienHunterCharacter.h"
#include "AlienHunter/Public/Structs/ItemData.h"
#include "Shooter.generated.h"

class AWeapon;
class AItem;

UCLASS()
class ALIENHUNTER_API AShooter : public AAlienHunterCharacter
{
	GENERATED_BODY()

	/**
	 * This component location is needed when we do line trace for checking
	 * if there would be collision with our weapon when aiming
	*/
	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartTraceComponent;

public:
	AShooter();

	virtual void Tick(float DeltaTime) override;

	/** 
	  * Checks if there is a possibility to aim using current weapon 
	  * If current used weapon is not valid, returns false
	  * If a shooter is either flying or falling, returns false
	  * If there would be a collions if the shooter tried aiming , returns false
	 */ 
	UFUNCTION(BlueprintCallable)
	virtual bool CanAim() const;

	/**
	  * Use a current used weapon to carry out a single attack
	  * if current used weapon is not valid, it will start melee fight
	 */
	virtual void TryToSingleAttack() override;

	/** Called in case of using auto fire gun insted of TryToSingleAttack */
	virtual void TryToAutoAttack(float AxisValue);

	/**
	  * Starts to try aiming
	  * @return true if aiming has been enabled
	 */
	FORCEINLINE bool TryToStartAiming()
	{
		return bIsAiming = CanAim();
	}

	/** Ends aiming */
	FORCEINLINE void EndAiming()
	{
		bIsAiming = false;
	}

	void SetCurrentUsedWeapon(AWeapon* const NewWeapon);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	/** Called when the current health reaches 0 */
	virtual void HandleDeath() override;

	/** If CurrentUsedWeapon both is Gun and has autofiremode, we can toggle between single fire and auto fire */
	void ChangeGunFireMode();

	/** If CurrentUsedWeapon is of type AGun, we can reload ammunition */
	void ReloadGun();

	/** Calculates Left hand transform if the character is holding the weapon of type rifle , called every tick */
	void CalculateLeftHandTransform();

	/** Drops current used weapon on the ground */
	void DropWeapon();

	/** 
	 * A weapon equiped by this function is not saved in the game instance 
	 * the weapon will be forgotten after restarting level and all the item/weapon data about it
	 * This function is desired to be used on AI characters which doesn't have an equipment
	 */
	UFUNCTION(BlueprintCallable)
	void EquipCustomWeapon(AWeapon* const Weapon);

protected:
	/** This is the actual weapon that player character holds in his hands */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
	AWeapon* CurrentUsedWeapon = nullptr;

	/** Weapon attack montages like rifle/pistol recoil and so on */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Combat")
	TMap<EWeaponType, UAnimMontage*> WeaponAttackMontages;

	/** A shooter can aim by using rifles, grenades and other available weapon */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	bool bIsAiming = false;

	/** Transform to attach a left hand to the rifle */
	UPROPERTY(BlueprintReadOnly)
	FTransform LeftHandTransform;
	
public:
	FORCEINLINE AWeapon* GetCurrentUsedWeapon() const
	{
		return CurrentUsedWeapon; 
	}

	/**
	 * @param WeaponType the type of the weapon for which we want a attack anim montage
	 * @return The montage related to the passed weapon type
	 */
	FORCEINLINE  UAnimMontage* GetWeaponAttackMontage(EWeaponType WeaponType) const
	{
		return WeaponAttackMontages.FindRef(WeaponType);
	}

};
