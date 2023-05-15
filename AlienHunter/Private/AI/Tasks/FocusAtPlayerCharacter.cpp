// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/FocusAtPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UFocusAtPlayerCharacter::UFocusAtPlayerCharacter()
{
	NodeName = TEXT("Focus At Player Character");
}

EBTNodeResult::Type UFocusAtPlayerCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Get Controller and Owner of the controller 
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	if (!IsValid(OwnerController))
	{
		return EBTNodeResult::Failed;
	}

	// Gets blackboard comp
	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	AActor* const FocusActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));
	OwnerController->SetFocus(FocusActor);
	return EBTNodeResult::Succeeded;
}
