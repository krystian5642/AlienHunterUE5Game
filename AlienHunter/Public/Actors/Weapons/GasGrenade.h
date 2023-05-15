// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/Grenade.h"
#include "Components/TimelineComponent.h"
#include "GasGrenade.generated.h"

UCLASS()
class ALIENHUNTER_API AGasGrenade : public AGrenade
{
	GENERATED_BODY()

	FTimeline GrenadeEffectsTimeline;

	/** We use this curve to scale effects like sound volume , the scale of the smoke cloud , damage radius and damage ammount*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ScaleEffectsCurve;

	/** Used to apply damage to actors over time */
	FTimerHandle RadialDamageHandle;

	/** Used to save initial damage params and scale them properly per time */
	UPROPERTY()
	FRadialDamageParams TargetRadialDamageParams;

public:
	AGasGrenade();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Called after activate delay time has passed */
	virtual void Activate() override;

	/** Called when the lifespan of an actor expires (if he has one). */
	virtual void LifeSpanExpired() override;

	/** Sets scale of all effects that this greande has */
	UFUNCTION()
	void SetEffectScale(float Scale);

protected:
	/** Smoke cloud life span in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects", meta = (ClampMin = 0.0f))
	float SmokeCloudLifeSpan = 10.0f;

private:
	void InitializeScaleEffectsTimeline();

};
