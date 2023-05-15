// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagazineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoChange, int, CurrentAmmo);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UMagazineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void ReloadAmmunition();

	/** 
	 * @return true if the magazine is empty
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsEmpty() const { return CurrentAmmoInMagazine == 0; }

	/** Function to subtract one bullet from CurrentAmmoInMagzaine */
	void SubtractBullet();

public:
	UPROPERTY(BlueprintAssignable)
	FOnCurrentAmmoChange OnCurrentAmmoChange;

protected:
	/** Max value of ammunition/energy than can reside in a Magazine */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Capacity", meta = (ClampMin = 0, AllowPrivateAccess = "true"))
	int MaxAmmoInMagazine = 40;

	/** Current value of ammunition/energy that is present in a Magazine right now */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Capacity", meta = (ClampMin = 0, AllowPrivateAccess = "true"))
	int CurrentAmmoInMagazine = 0;

	/** Set initial the gun reload speed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magazine", meta = (ClampMin = 0.f, AllowPrivateAccess = "true"))
	float MagazineReloadTime = 0.5f;

public:	
	FORCEINLINE int GetMaxAmmoInMagazine() const 
	{ 
		return MaxAmmoInMagazine; 
	}
	FORCEINLINE void SetMaxAmmoInMagazine(int NewMaxAmmoInMagazine) 
	{
		MaxAmmoInMagazine = NewMaxAmmoInMagazine; 
	}

	FORCEINLINE float GetCurrentAmmoInMagazine() const 
	{
		return CurrentAmmoInMagazine; 
	}

	FORCEINLINE void SetMagazineReloadTime(float NewMagazineReloadTime) 
	{ 
		MagazineReloadTime = NewMagazineReloadTime; 
	}
	FORCEINLINE float GetMagazineReloadTime() const 
	{ 
		return MagazineReloadTime; 
	}
		
};
