// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/BobbingComponent.h"

// Sets default values for this component's properties
UBobbingComponent::UBobbingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called every frame
void UBobbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float Alpha = (MaxSpeed / Amplitude) * GetOwner()->GetGameTimeSinceCreation();
	const float CosAlpha = FMath::Sin(Alpha);
	const float Offset = Amplitude * CosAlpha;
	const FVector CurrentLocation = GetOwner()->GetActorLocation();
	const FVector NewLocation = StartLocation + FVector(0, 0, Offset);
	GetOwner()->SetActorLocation(NewLocation);
}

void UBobbingComponent::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetOwner()->GetActorLocation();
}

