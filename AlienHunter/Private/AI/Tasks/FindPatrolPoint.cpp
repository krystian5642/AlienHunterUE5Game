// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/FindPatrolPoint.h"
#include "AI/Controllers/AIAdvancedAlienController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Actors/DifferentActors/PatrolPath.h"

UFindPatrolPoint::UFindPatrolPoint()
{
	NodeName = TEXT("Find Patrol Point");
}

EBTNodeResult::Type UFindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Gets Controller and Owner of the controller 
	AAIAdvancedAlienController* const AdvancedAIController = Cast<AAIAdvancedAlienController>(OwnerComp.GetAIOwner());
	if (!AdvancedAIController)
	{
		return EBTNodeResult::Failed;
	}

	if (!AdvancedAIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	const APatrolPath* const PatrolPath = AdvancedAIController->GetPatrolPath();
	if (!PatrolPath)
	{
		return EBTNodeResult::Failed;
	}

	// Gets blackboard comp
	UBlackboardComponent* const BlackboardComp = AdvancedAIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	int32 CurrentIndex = BlackboardComp->GetValueAsInt(LastPatrolPointIndex.SelectedKeyName);
	int32 PatrolPointsNum = PatrolPath->GetPatrolPointsNum();
	EMoveDirection AIMoveDirection = static_cast<EMoveDirection>(BlackboardComp->GetValueAsEnum(MoveDirection.SelectedKeyName));

	if (AIMoveDirection == EMoveDirection::Forward && CurrentIndex == (PatrolPointsNum-1))
	{
		AIMoveDirection = EMoveDirection::Backward;
		BlackboardComp->SetValueAsEnum(MoveDirection.SelectedKeyName, static_cast<uint8>(AIMoveDirection));
	}
	else if (AIMoveDirection == EMoveDirection::Backward && CurrentIndex == 0)
	{
		AIMoveDirection = EMoveDirection::Forward;
		BlackboardComp->SetValueAsEnum(MoveDirection.SelectedKeyName, static_cast<uint8>(AIMoveDirection));
	}

	CurrentIndex = AIMoveDirection == EMoveDirection::Forward ? ++CurrentIndex : --CurrentIndex;


	CurrentIndex = FMath::Clamp(CurrentIndex,0, PatrolPointsNum - 1);

	const TArray<FVector>& PatrolPoints = PatrolPath->GetPatrolPoints();
	const FVector PatrolPointLocal = PatrolPoints[CurrentIndex];
	const FVector PatrolPointWorld = PatrolPath->GetActorTransform().TransformPosition(PatrolPointLocal);

	BlackboardComp->SetValueAsInt(LastPatrolPointIndex.SelectedKeyName, CurrentIndex);
	BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), PatrolPointWorld);

	return EBTNodeResult::Succeeded;
}
