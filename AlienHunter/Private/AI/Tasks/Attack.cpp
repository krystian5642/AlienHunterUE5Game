// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/Attack.h"
#include "AlienHunter/AlienHunterCharacter.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

UAttack::UAttack()
{
	bNotifyTick = true;
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp,NodeMemory);

	// Get Controller and Owner of the controller 
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return EBTNodeResult::Failed;
	}
	AAlienHunterCharacter* const AlienHunterCharacter = Cast<AAlienHunterCharacter>(OwnerController->GetPawn());
	if (!AlienHunterCharacter)
	{
		return EBTNodeResult::Failed;
	}

	if (AlienHunterCharacter->IsPlayingAnyMontageOnMesh())
	{
		return EBTNodeResult::Succeeded;
	}

	AlienHunterCharacter->TryToSingleAttack();

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
