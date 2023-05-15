// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/FindPlayerLocation.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationData.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"

UFindPlayerLocation::UFindPlayerLocation()
{
	NodeName = TEXT("Find Player Location");
	bNotifyTick = true;
}

void UFindPlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Gets Controller and Owner of the controller 
	AAIController* const OwnerController = OwnerComp.GetAIOwner();
	ACharacter* const PlayerCharacter = UGameplayStatics::GetPlayerCharacter(OwnerController, 0);
	if (!OwnerController || !PlayerCharacter)
	{
		return;
	}

	// Gets blackboard comp
	UBlackboardComponent* const BlackboardComp = OwnerController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}

	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	ANavigationData* const NavData = UNavigationSystemV1::GetCurrent(GetWorld())->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	FNavLocation NavLoc;
	if (NavData)
	{
		NavData->ProjectPoint(PlayerLocation, NavLoc, FVector(1000));
		PlayerLocation = NavLoc.Location;
		BlackboardComp->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLocation);
	}
}
