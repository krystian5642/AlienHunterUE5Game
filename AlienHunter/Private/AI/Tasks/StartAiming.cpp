// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/StartAiming.h"
#include "AlienHunter/Public/Actors/Characters/Shooter.h"
#include "AIController.h"

UStartAiming::UStartAiming()
{
	NodeName = TEXT("Start Aiming");
}

EBTNodeResult::Type UStartAiming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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
	return Shooter->TryToStartAiming() ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
