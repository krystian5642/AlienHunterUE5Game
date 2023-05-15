// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/AIAdvancedAlienController.h"
#include "Actors/DifferentActors/PatrolPath.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AAIAdvancedAlienController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!IsValid(InPawn))
	{
		return;
	}
	FindClosestPatrolPath();

	UBlackboardComponent* const BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsInt("PatrolPathIndex",-1);
	}
}

void AAIAdvancedAlienController::FindClosestPatrolPath()
{
	TArray<AActor*> PatrolPaths;
	UGameplayStatics::GetAllActorsOfClass(this, APatrolPath::StaticClass(), PatrolPaths);

	if (PatrolPaths.IsEmpty())
	{
		return;
	}
	float Distance;
	AActor* const NearestActor = UGameplayStatics::FindNearestActor(GetPawn()->GetActorLocation(), PatrolPaths, Distance);
	
	PatrolPath = Cast<APatrolPath>(NearestActor);
}
