// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EndAiming.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API UEndAiming : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UEndAiming();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
