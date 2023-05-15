// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/EndAiming.h"
#include "AlienHunter/Public/Actors/Characters/Shooter.h"
#include "AIController.h"

UEndAiming::UEndAiming()
{
	NodeName = TEXT("End Aiming");
}

EBTNodeResult::Type UEndAiming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Get Controller and Owner of the controller 
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return EBTNodeResult::Failed;
	}
	AShooter* const Shooter = Cast<AShooter>(OwnerController->GetPawn());
	if (!Shooter)
	{
		return EBTNodeResult::Failed;
	}
	Shooter->EndAiming();
	return EBTNodeResult::Succeeded;

}
