// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "StartAiming.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API UStartAiming : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UStartAiming();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
