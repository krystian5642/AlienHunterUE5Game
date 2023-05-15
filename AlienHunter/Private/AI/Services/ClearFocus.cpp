// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/ClearFocus.h"
#include "AIController.h"

UClearFocus::UClearFocus()
{
	NodeName = TEXT("Clear Focus");
	bNotifyBecomeRelevant = true;
}

void UClearFocus::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// Get Controller
	AAIController* const OwnerController = OwnerComp.GetAIOwner();

	if (!OwnerController)
	{
		return;
	}
	OwnerController->ClearFocus(EAIFocusPriority::Gameplay);
}