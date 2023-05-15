// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "ProjectilePredictionSpline.generated.h"

class USplineComponent;
class AGrenade;

UCLASS()
class ALIENHUNTER_API AProjectilePredictionSpline : public AActor
{
	GENERATED_BODY()

	/** Spline Component of this actor */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;

	/** Template spline mesh component used to create new object based on it */
	UPROPERTY(VisibleDefaultsOnly, Category = "Spline")
	USplineMeshComponent* TemplateSplineMeshComponent;

	/** Array of the spline mesh components */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	TArray<USplineMeshComponent*> SplineMeshComponents;

	/** End point of the spline component , visible only if something was hit */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spline", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PredictionEndPoint;

public:	
	// Sets default values for this actor's properties
	AProjectilePredictionSpline();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Called during predicting path of projectile in tick function */
	void UpdateSplinePoints();

	/**
	 * Called to start predicitng projectile path if it is set 
	 * @param InGrenade The grenade to predict its path
	 * @param DelayTime Time to blend to the aim pose
	 */
	void StartPredictingPath(AGrenade* const InGreande,float DelayTime);

	/** Called to end predicitng projectile path */
	void EndPredictingPath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	USplineMeshComponent* CreateSplineMeshComponent();

protected:
	/** The projectile (greande) we predict path */
	UPROPERTY()
	AGrenade* Grenade = nullptr;

private:
	/** Sets spline meshes both start and end location along the spline component */
	void SetStartAndEndOfSplineMeshes();

	/** 
	 * Finds spline points 
	 * @param OutLocations They are spline points of projectile path
	 * @param HitLocation End of the grenade trajectory
	 * @return true if something was hit
	*/
	bool FindProjectilePath(TArray<FVector>& OutLocations, FVector& HitLocation);

	/** Called after the delay time in the function StartPredictingPath has passed */
	void StartPredictingPathWithDelay();

public:
	FORCEINLINE USplineComponent* GetSplineComponent() const 
	{ 
		return SplineComponent; 
	};

};
