// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/Item.h"
#include "AlienHunter/Public/Structs/MaterialReaction.h"
#include "Weapon.generated.h"


class AShooter;
class AAlienHunterCharacter;

UCLASS()
class ALIENHUNTER_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	/** 
	 * Function to start attacking by the owner of this weapon , default implementation just plays weapon atack montages
	 * @return weapon attack lenght , if 0.0 attack was impossible
	 */
	virtual float WeaponAttack();

	/** Function to start using/holding the weapon */
	virtual void Use(AAlienHunterCharacter* Character) override;

protected:
	virtual void BeginPlay() override;

	/** 
	 * Called while owner is carrying out an attack using this weapon
	 * @return the lenght of the anim monatge
	 */
	float PlayWeaponAttackMontageOnOwner();

	/** Play when any action that requires both sound and visula effects has been taken */
	virtual void TriggerSoundAndVisuals() {};

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Creates Text info about this item */
	virtual void CreateItemTextInfo() override;

	/** Applys damage to the actors */
	virtual void ApplyDamage() {};

	/** 
	 * Spawns proper sound and visual effects on a physical surface type whenever it's needed
	 * @param HitResult HitData that contain data about the physical surface type, on which we want to spawn effects  
	 */
	void SpawnSurfaceReaction(const FHitResult& HitResult);

protected:
	/** We will use this socket to attach this weapon to the character mesh */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Sockets")
	FName ParentSocket;

	/** Max Damage taken in one attack / shot */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat",meta=(ClampMin = 0.0f))
	float MaxDamage = 0;

	/** Min Damage taken in one attack / shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = 0.0f))
	float MinDamage = 0;

	UPROPERTY()
	AShooter* OwnerAsShooter;

	/** The subclass of the damage type caused by this weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass = UDamageType::StaticClass();

	/** Used to spawn a proper both sound and visual effects on a physical surface type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TMap<TEnumAsByte<EPhysicalSurface>, FMaterialReaction> MaterialReactions;

public:
	/** Sets and returns ParentSocket */
	FORCEINLINE const FName& GetParentSocket() const 
	{
		return ParentSocket; 
	}
	FORCEINLINE void SetParentSocket(const FName& Socket) 
	{
		ParentSocket = Socket;
	}

	/** Sets and returns damage */
	FORCEINLINE float GetMaxDamage() const 
	{ 
		return MaxDamage; 
	}
	FORCEINLINE void SetMaxDamage(float NewMaxDamage) 
	{
		MaxDamage = FMath::Max(NewMaxDamage, 0);
	}

	/** Sets and returns damage */
	FORCEINLINE float GetMinDamage() const
	{
		return MinDamage;
	}
	FORCEINLINE void SetMinDamage(float NewMinDamage)
	{
		MinDamage = FMath::Clamp(NewMinDamage,0,MaxDamage);
	}

};
