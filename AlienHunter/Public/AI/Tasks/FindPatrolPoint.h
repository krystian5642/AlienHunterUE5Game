// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "FindPatrolPoint.generated.h"

UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
	Forward,
	Backward
};

UCLASS()
class ALIENHUNTER_API UFindPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UFindPatrolPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastPatrolPointIndex;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MoveDirection;
	
};
