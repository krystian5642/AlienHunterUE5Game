// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/IsPlayerMoving.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

UIsPlayerMoving::UIsPlayerMoving()
{
	NodeName = TEXT("Is Player Moving");
}

bool UIsPlayerMoving::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bSuccess = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (!bSuccess) return false;

	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController)
	{
		return false;
	}

	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}

	const ACharacter* const Character = Cast<ACharacter>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));
	if (!Character)
	{
		return false;
	}

	return Character->GetCharacterMovement()->IsMovingOnGround();

}
