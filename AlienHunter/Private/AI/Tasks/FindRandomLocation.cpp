// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/FindRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "AIController.h"

UFindRandomLocation::UFindRandomLocation()
{
	NodeName = TEXT("Find Random Location");
}

EBTNodeResult::Type UFindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Gets Controller
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return EBTNodeResult::Failed;
	}

	// Gets controlled pawn
	APawn* ControlledPawn = OwnerController->GetPawn();

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	// Gets blackboard comp
	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Sets random location
	FNavLocation Loc;
	UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return EBTNodeResult::Failed;
	}
	NavSys->GetRandomPointInNavigableRadius(BlackboardComp->GetValueAsVector(StartLocation.SelectedKeyName), SearchRadius, Loc);
	BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);

	// finish task 
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}

