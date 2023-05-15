// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/ProjectilePredictionSpline.h"
#include "Components/SplineComponent.h"
#include "Actors/Weapons/Grenade.h"
#include "Kismet/GameplayStaticsTypes.h"

// Sets default values
AProjectilePredictionSpline::AProjectilePredictionSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(SplineComponent);

	TemplateSplineMeshComponent = CreateDefaultSubobject<USplineMeshComponent>(TEXT("TemplateSplineMeshComp"));

	PredictionEndPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PredictionEndPoint"));
	PredictionEndPoint->SetupAttachment(SplineComponent);

}

// Called every frame
void AProjectilePredictionSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSplinePoints();
}

void AProjectilePredictionSpline::UpdateSplinePoints()
{
	if (PredictionEndPoint && !IsValid(Grenade))
	{
		return;
	}

	TArray<FVector> SplinePoints;
	FVector HitLocation(0,0,0);
	const bool bWasHitSomething = FindProjectilePath(SplinePoints,HitLocation);

	// Sets prediction end point
	PredictionEndPoint->SetWorldLocation(HitLocation, false, nullptr, ETeleportType::TeleportPhysics);
	PredictionEndPoint->SetVisibility(bWasHitSomething);

	SplineComponent->SetSplinePoints(SplinePoints, ESplineCoordinateSpace::Type::World);
	int32 NumPoints = SplinePoints.Num();

	while (SplineMeshComponents.Num() < NumPoints-1)
	{
		SplineMeshComponents.Add(CreateSplineMeshComponent());
	}

	SetStartAndEndOfSplineMeshes();
}

void AProjectilePredictionSpline::StartPredictingPath(AGrenade* const InGreande, float DelayTime)
{
	Grenade = InGreande;
	FTimerHandle StartPredictingDelay;
	GetWorldTimerManager().SetTimer(StartPredictingDelay,this,&AProjectilePredictionSpline::StartPredictingPathWithDelay, DelayTime, false);
}

void AProjectilePredictionSpline::EndPredictingPath()
{
	Grenade = nullptr;
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	if (PredictionEndPoint)
	{
		PredictionEndPoint->SetVisibility(false);
	}
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

// Called when the game starts or when spawned
void AProjectilePredictionSpline::BeginPlay()
{
	Super::BeginPlay();
	TemplateSplineMeshComponent->SetVisibility(false);
}

USplineMeshComponent* AProjectilePredictionSpline::CreateSplineMeshComponent()
{
	// Creates spline mesh component based on template spline mesh component
	USplineMeshComponent* SplineMeshComp = NewObject<USplineMeshComponent>
		(
			this,
			USplineMeshComponent::StaticClass(),
			NAME_None,
			EObjectFlags::RF_NoFlags,
			TemplateSplineMeshComponent
		);
	if (!SplineMeshComp)
	{
		return nullptr;
	}
	SplineMeshComp->CreationMethod = EComponentCreationMethod::Native;
	SplineMeshComp->RegisterComponent();
	SplineMeshComp->SetVisibility(true);

	if (SplineComponent)
	{
		SplineMeshComp->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	// Returns component
	return SplineMeshComp;
}

void AProjectilePredictionSpline::SetStartAndEndOfSplineMeshes()
{
	if (!SplineComponent || !PredictionEndPoint)
	{
		return;
	}

	int Index = 0;
	int SplinePointNum = SplineComponent->GetNumberOfSplinePoints();
	for (auto& SplineMeshComp : SplineMeshComponents)
	{
		const FVector StartPosition = SplineComponent->GetLocationAtSplinePoint(Index, ESplineCoordinateSpace::Type::Local);
		const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::Type::Local);
		const FVector EndPosition = SplineComponent->GetLocationAtSplinePoint(Index + 1, ESplineCoordinateSpace::Type::Local);
		const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(Index + 1, ESplineCoordinateSpace::Type::Local);
		SplineMeshComp->SetStartAndEnd(StartPosition, StartTangent, EndPosition, EndTangent);
		++Index;
	}
}

bool AProjectilePredictionSpline::FindProjectilePath(TArray<FVector>& OutLocations,FVector& HitLocation)
{
	OutLocations.Reset();
	if (!IsValid(Grenade))
	{
		return false;
	}
	FPredictProjectilePathResult PredictResult;
	const bool bWasHitSomething = Grenade->PredictGrenadePath(PredictResult);
	for (const auto& PathData : PredictResult.PathData)
	{
		OutLocations.Add(PathData.Location);
	}
	HitLocation = PredictResult.HitResult.ImpactPoint;
	return bWasHitSomething;
}

void AProjectilePredictionSpline::StartPredictingPathWithDelay()
{
	if (IsValid(Grenade))
	{
		SetActorTickEnabled(true);
		SetActorHiddenInGame(false);
	}
}


