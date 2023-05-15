// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/Weapon.h"
#include "Grenade.generated.h"

class AShooter;
struct FPredictProjectilePathResult;

UCLASS()
class ALIENHUNTER_API AGrenade : public AWeapon
{
	GENERATED_BODY()

	FTimerHandle AddImpulseDelayTimer;

	FTimerHandle ActivateDelayTimer;

	/** Direction in which the grenade is going to be thrown */
	FVector ThrowDirection;

	/** Spawned when the grenade has just being activated */
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	USoundBase* GrenadeSound;

	/** Spawned when the grenade has just been activated */
	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	UParticleSystem* GrenadeVisual;

public:
	/** Max scale of the explosion or the smoke/gas cloud */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects", meta = (ClampMin = 0.0f))
	float VisualEffectMaxScale = 1.0f;

public:
	// Sets default values for this actor's properties
	AGrenade();

	virtual void Tick(float DeltaTime) override;

	/** 
	 * Function to start throwing the grenade
	 */
	virtual float WeaponAttack() override;

	/** true if was something hit */
	bool PredictGrenadePath(FPredictProjectilePathResult& PredictResult);

protected:
	virtual void BeginPlay() override;

	/** Called when any action that requires both sound and visula effects has been taken */
	virtual void TriggerSoundAndVisuals() override;

	/** Applys damage to all actors in the radial radius */
	virtual void ApplyDamage() override;

	/** Called after activate delay time has passed */
	virtual void Activate();

	/** Function to activate movement of the greande */
	void AddImpulseDelay();

	/** Funtion to start throwing the greande */
	float ThrowGrenade();

	/** Searches for a ground to spawn a surface reacton */
	bool GrenadeGroundTrace(FHitResult& HitResult);


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Creates Text info about this item */
	virtual void CreateItemTextInfo() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Combat", meta = (ClampMin = 0.0f))
	float DamageInnerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat",meta = (ClampMin = 0.0f))
	float DamageOuterRadius;

	/** Sets default value ActivateDelayTime for zero */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (ClampMin = 0))
	float ActivateDelayTime;

	/** Rotates camera forward vector Pitch by this angle to adjust throw direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float AimTargetOffsetAngle = 10.0f;
	
	UPROPERTY()
	UAudioComponent* GrenadeSoundComponent = nullptr;

	UPROPERTY()
	UParticleSystemComponent* GrenadeVisualComponent = nullptr;

private:
	/** Called before a grenade is going to be thrown */
	void EnablePhysicsOnMesh();

	/** Updates thrown direction */
	void UpdateThrowDirection(bool bHasThrowingStarted = false);

public:
	FORCEINLINE float GetExplodeDelayTime() const 
	{ 
		return ActivateDelayTime; 
	}
	FORCEINLINE void SetExplodeDelayTime(float NewExplodeDelayTime) 
	{ 
		ActivateDelayTime = FMath::Min(0, NewExplodeDelayTime);
	}
};
