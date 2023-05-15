// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "AlienHunter/AlienHunterCharacter.h"
#include "ChangeMovementMode.generated.h"

UCLASS()
class ALIENHUNTER_API UChangeMovementMode : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UChangeMovementMode();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere,Category = "Movement")
	EBasicMovementMode MovementMode = EBasicMovementMode::Standing;

};
