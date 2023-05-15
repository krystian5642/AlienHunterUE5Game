// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/AIPrimitiveAlienController.h"
#include "AIAdvancedAlienController.generated.h"

class APatrolPath;

UCLASS()
class ALIENHUNTER_API AAIAdvancedAlienController : public AAIPrimitiveAlienController
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Patrol Path", meta = (AllowPrivateAccess = "true"))
	APatrolPath* PatrolPath;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	/** Finds the closest patrol path from all available patrol path in the world */
	void FindClosestPatrolPath();

public:
	FORCEINLINE APatrolPath* GetPatrolPath() const
	{
		return PatrolPath;
	}
};
