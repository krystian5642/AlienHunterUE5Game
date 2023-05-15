// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/SetPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

USetPlayerCharacter::USetPlayerCharacter()
{
	NodeName = TEXT("Set Player Character");
	bNotifyBecomeRelevant = true;
}

void USetPlayerCharacter::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	// Get Controller and player pawn
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	ACharacter* const PlayerCharacter = UGameplayStatics::GetPlayerCharacter(OwnerController, 0);
	if (!OwnerController || !IsValid(PlayerCharacter))
	{
		return;
	}

	// Get blackboard compoenent and controlled actor
	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), PlayerCharacter);

}
