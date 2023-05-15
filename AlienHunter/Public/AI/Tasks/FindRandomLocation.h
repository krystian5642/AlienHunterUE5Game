// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "FindRandomLocation.generated.h"

UCLASS()
class ALIENHUNTER_API UFindRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UFindRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	/** Search radius */
	UPROPERTY(EditAnywhere, Category = "Radius",meta = (ClampMin = 0.0f))
	float SearchRadius = 1000.0f;

	/** Searchs for points around this location */
	UPROPERTY(EditAnywhere,Category="Blackboard")
	FBlackboardKeySelector StartLocation;
	
};
