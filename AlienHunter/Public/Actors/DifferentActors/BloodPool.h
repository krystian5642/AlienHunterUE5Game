// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "Components/TimelineComponent.h"
#include "BloodPool.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API ABloodPool : public ADecalActor
{
	GENERATED_BODY()

	FTimeline GrowthTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Blood Pool")
	UCurveFloat* GrowthCurve;

	/** Used to create a material instance dynamic */
	UPROPERTY()
	UMaterial* BloodMaterial;

public:
	ABloodPool();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** AnimationPhase is from 0 to 1 */
	UFUNCTION()
	void SetAnimationPhase(float AnimationPhase);

public:
	/** Time that takes to growth to the full size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Blood Pool")
	float GrowthTime = 1.0f;

private:
	void InitializeGrowthTimeline();
	
};
