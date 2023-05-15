// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/Weapon.h"
#include "Gun.generated.h"

class UMagazineComponent;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SingleFire,
	AutoFire
};

UCLASS()
class ALIENHUNTER_API AGun : public AWeapon
{
	GENERATED_BODY()

	/** Active while reloading ammunition */
	FTimerHandle HandleReloadTimer;

	/** Active between shoots*/
	FTimerHandle HandleFireDelay;

	/** Played while the owner is reloading */
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	USoundBase* ReloadSound;

	/** Played while the owner shooting */
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	USoundBase* ShootSound;

	/** Spawned at the top of the gun muzzle */
	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Visual Effects", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* BulletTrail;

	/** Magazine component of the gun */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Magazine", meta = (AllowPrivateAccess = "true"))
	UMagazineComponent* MagazineComponent;

public:
	// Set default values for this actor's properties
	AGun();

	virtual float WeaponAttack() override;

	void ReloadMagazine();

	/** true if fire mode was successfully changed */
	bool ToggleFireMode();

	/** true if the gun is still reloading magazine */
	FORCEINLINE bool IsReloading() const
	{
		return GetWorldTimerManager().IsTimerActive(HandleReloadTimer);
	}

	/** true if the gun is in a delay state between shoots */
	FORCEINLINE bool IsFireDelay() const
	{
		return GetWorldTimerManager().IsTimerActive(HandleFireDelay);
	}

protected:
	/** Plays when any action that requires both sound and visual effects was taken */
	virtual void TriggerSoundAndVisuals() override;

	/** Creates Text info about this item */
	virtual void CreateItemTextInfo() override;

	/** Applys damage to the actors */
	virtual void ApplyDamage() override;

	/** Triggers bullet trail effect */
	UFUNCTION(BlueprintImplementableEvent)
	void TriggerBulletTrailEffect();

protected:
	/** Sets initial the gun fire rate */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Stats")
	float FireRate = 0.5f;

	/** true if we can change default fire mode when the game is running */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	bool bCanToggleFireMode = false;

	/** Set Default fire mode to single fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	EFireMode FireMode = EFireMode::SingleFire;

	/** The last hit location that was hit by the bullet */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FVector LastHitLocation;

	/** The range in which this weapon can apply damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRange = 6000.0f;

private:
	/**
	* @param ShootDirection The direction from which the bullet came from 
	* @param EndPoint If was hit something, it is a impact point, otherwise end trace point
	* @param HitResult the hit result of the gun trace 
	* @return true if something was hit by the bullet trace
	*/
	bool GunTrace(FVector& ShootDirection, FVector & EndPoint,FHitResult& HitResult);

public:
	FORCEINLINE float GetFireRate() const 
	{ 
		return FireRate;
	}
	FORCEINLINE void SetFireRate(float NewFireRate) 
	{ 
		FireRate = NewFireRate; 
	}

	FORCEINLINE EFireMode GetFireMode() const 
	{ 
		return FireMode; 
	}

};
