// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "SetPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API USetPlayerCharacter : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	USetPlayerCharacter();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
