// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/ChangeMovementMode.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

UChangeMovementMode::UChangeMovementMode()
{
	NodeName = TEXT("Change Movement Mode");
	bNotifyBecomeRelevant = true;
}

void UChangeMovementMode::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!IsValid(OwnerController))
	{
		return;
	}
	AAlienHunterCharacter* const ControlledCharacter = Cast<AAlienHunterCharacter>(OwnerController->GetPawn());

	if (IsValid(ControlledCharacter))
	{
		ControlledCharacter->SetCurrentMovementMode(MovementMode);
	}
}
