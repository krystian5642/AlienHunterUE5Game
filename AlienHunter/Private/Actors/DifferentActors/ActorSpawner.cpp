// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/ActorSpawner.h"
#include "TimerManager.h"

void AActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(SpawnActorHandle,this,&AActorSpawner::SpawnActor,SpawnInterval);
}

void AActorSpawner::SpawnActor()
{
	if (SpawnActorClass && bShouldSpawnActors)
	{
		if (SpawnedActors.Num() < MaxActorNumberAtOnce)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AActor* const Actor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, GetActorTransform(), SpawnParams);
			Actor->OnDestroyed.AddDynamic(this,&AActorSpawner::OnActorDestroyed);
			SpawnedActors.Add(Actor);
		}
	}

	//In case, spawn interval changed
	GetWorldTimerManager().SetTimer(SpawnActorHandle, this, &AActorSpawner::SpawnActor, SpawnInterval);
}

void AActorSpawner::OnActorDestroyed(AActor* Actor)
{
	SpawnedActors.Remove(Actor);
}

void AActorSpawner::BeginDestroy()
{
	Super::BeginDestroy();
	for (auto& Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}	
	}
}
