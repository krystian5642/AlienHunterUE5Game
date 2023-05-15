// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/ClearBlackboardKeyOnEnd.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UClearBlackboardKeyOnEnd::UClearBlackboardKeyOnEnd()
{
	NodeName = TEXT("Clear Blackboard Value On End");
	bNotifyCeaseRelevant = true;
}

void UClearBlackboardKeyOnEnd::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp,NodeMemory);

	// Get Controller
	AAIController* const OwnerController = OwnerComp.GetAIOwner();

	if (!OwnerController)
	{
		return;
	}

	// Get blackboard comp
	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	if (!GetSelectedBlackboardKey().IsValid())
	{
		return;
	}

	BlackboardComp->ClearValue(GetSelectedBlackboardKey());
}
