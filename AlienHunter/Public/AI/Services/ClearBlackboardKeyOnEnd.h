// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "ClearBlackboardKeyOnEnd.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API UClearBlackboardKeyOnEnd : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UClearBlackboardKeyOnEnd();

protected:
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
