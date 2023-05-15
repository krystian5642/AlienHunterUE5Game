// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API UClearFocus : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UClearFocus();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
