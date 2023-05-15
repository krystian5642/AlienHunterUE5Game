// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "IsPlayerMoving.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API UIsPlayerMoving : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UIsPlayerMoving();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
