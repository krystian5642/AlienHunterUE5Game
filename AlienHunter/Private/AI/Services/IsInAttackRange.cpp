// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/IsInAttackRange.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UIsInAttackRange::UIsInAttackRange()
{
	NodeName = TEXT("Is In Attack Range");
	bNotifyTick = true;
}

void UIsInAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Controller and player pawn
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	APawn* const PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!OwnerController || !IsValid(PlayerPawn))
	{
		return;
	}

	// Get blackboard compoenent and controlled actor
	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	AActor* const ControlledActor = OwnerController->GetPawn();
	if (!BlackboardComp || !IsValid(ControlledActor))
	{
		return;
	}
	const bool bIsInAttackRange = ControlledActor->GetDistanceTo(PlayerPawn) <= AttackRange;
	BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), bIsInAttackRange);
}
