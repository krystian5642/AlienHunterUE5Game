// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class ALIENHUNTER_API APatrolPath : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MakeEditWidget = "true", AllowPrivateAccess = "true"))
	TArray<FVector> PatrolPoints;

public:
	FORCEINLINE const TArray<FVector>& GetPatrolPoints() const
	{
		return PatrolPoints;
	}
	FORCEINLINE int32 GetPatrolPointsNum() const
	{
		return PatrolPoints.Num();
	}

};
