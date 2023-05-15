// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API UIsInAttackRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UIsInAttackRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	/** The minimal distance from which an attack can be started */
	UPROPERTY(EditAnywhere, Category = "Range",meta = (ClampMin =0.0f))
	float AttackRange = 100.0f;
};
