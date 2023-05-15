// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawner.generated.h"

UCLASS()
class ALIENHUNTER_API AActorSpawner : public AActor
{
	GENERATED_BODY()

	FTimerHandle SpawnActorHandle;

protected:
	virtual void BeginPlay() override;

	void SpawnActor();

	UFUNCTION()
	void OnActorDestroyed(AActor* Actor);

	virtual void BeginDestroy() override;

protected:
	/*
	* Less than zero means there is no limit of actors spawned at once
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxActorNumberAtOnce = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, ForceUnits = "s"))
	float SpawnInterval = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> SpawnedActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldSpawnActors = true;

};
