// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/BloodPool.h"
#include "Components/DecalComponent.h"

ABloodPool::ABloodPool()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	GetDecal()->DecalSize = FVector(2,80,80);
}

void ABloodPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GrowthTimeline.IsPlaying())
	{
		GrowthTimeline.TickTimeline(DeltaTime);
	}
}

void ABloodPool::BeginPlay()
{
	Super::BeginPlay();
	InitializeGrowthTimeline();
	GrowthTimeline.PlayFromStart();
	float LifeSpan = 0.0f;
	if (Owner)
	{
		LifeSpan = GrowthTime + Owner->GetLifeSpan();
	}
	SetLifeSpan(LifeSpan);
	if (GetDecalMaterial())
	{
		BloodMaterial = GetDecalMaterial()->GetMaterial();
	}
}

void ABloodPool::SetAnimationPhase(float AnimationPhase)
{
	if (!BloodMaterial)
	{
		return;
	}
	UMaterialInstanceDynamic* const DynamicMaterial = UMaterialInstanceDynamic::Create(BloodMaterial, this);
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("AnimationPhase"), AnimationPhase);
	}
	DynamicMaterial->GetMaterial();
	GetDecal()->SetDecalMaterial(DynamicMaterial);
}

void ABloodPool::InitializeGrowthTimeline()
{
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("SetAnimationPhase"));
	GrowthTimeline.AddInterpFloat(GrowthCurve, ProgressFunction);
	GrowthTimeline.SetTimelineLengthMode(TL_LastKeyFrame);

	/** Because the base timeline lasts 1 seconds */
	const float TimelinePlayRate = 1.0f / GrowthTime;
	GrowthTimeline.SetPlayRate(TimelinePlayRate);
}
