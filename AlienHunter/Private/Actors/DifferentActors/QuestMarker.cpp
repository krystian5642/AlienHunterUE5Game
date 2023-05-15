// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/QuestMarker.h"
#include "Components/QuestLogComponent.h"
#include "Actors/Characters/Cyborg.h"
#include "Kismet/GameplayStatics.h"
#include "AlienHunter/Public/Structs/QuestData.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Actors/DifferentActors/QuestTracker.h"

AQuestMarker::AQuestMarker()
{
	
}

void AQuestMarker::OnQuestBeginTracked(AQuestTracker* const Quest)
{
	if (IsValid(Quest) && Quest->GetCurrentObjectiveData().ObjectiveID.IsEqual(StartAtObjectiveID))
	{
		StartTrackingMarker();
	}
	else
	{
		EndTrackingMarker();
	}
}

void AQuestMarker::OnQuestEndTracked(AQuestTracker* const Quest)
{
	if (IsValid(Quest) && Quest->GetCurrentObjectiveData().ObjectiveID.IsEqual(StartAtObjectiveID))
	{
		EndTrackingMarker();
	}
}

void AQuestMarker::OnQuestObjectiveBegin(const FObjectiveData& ObjectiveData, bool bIsTracked)
{
	if (StartAtObjectiveID.IsEqual(ObjectiveData.ObjectiveID))
	{
		ShowAndActiveAllAttachedActors();
		if (bIsTracked)
		{
			StartTrackingMarker();
		}
	}
}

void AQuestMarker::OnQuestObjectiveEnd(const FObjectiveData& ObjectiveData)
{
	if (StartAtObjectiveID.IsEqual(ObjectiveData.ObjectiveID))
	{
		Destroy();
	}
}

void AQuestMarker::BeginPlay()
{
	Super::BeginPlay();

	if (ACyborg* const CyborgPlayer = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		PlayerQuestLog = CyborgPlayer->GetQuestLogComponent();
		if (PlayerQuestLog)
		{
			PlayerQuestLog->OnQuestObjectiveBegin.AddDynamic(this,&AQuestMarker::OnQuestObjectiveBegin);
			PlayerQuestLog->OnQuestObjectiveEnd.AddDynamic(this,&AQuestMarker::OnQuestObjectiveEnd);
			PlayerQuestLog->OnQuestBeginTracked.AddDynamic(this,&AQuestMarker::OnQuestBeginTracked);
			PlayerQuestLog->OnQuestEndTracked.AddDynamic(this,&AQuestMarker::OnQuestEndTracked);
		}
	}

	HideAndDeactiveAllAttachedActors();
	EndTrackingMarker();

	if (PlayerQuestLog)
	{
		AQuestTracker* ActiveQuest = PlayerQuestLog->GetActiveQuest();
		if (IsValid(ActiveQuest))
		{
			OnQuestObjectiveBegin(ActiveQuest->GetCurrentObjectiveData(), ActiveQuest->bIsTracked);
		}

		FTimerHandle CompletedQuestTimerHandle;
		GetWorldTimerManager().SetTimer(CompletedQuestTimerHandle, this, &AQuestMarker::HandleCompletedQuestObjectives, 1);
	}
}

void AQuestMarker::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACyborg* const PlayerCharacter = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter != OtherActor || OtherComp != PlayerCharacter->GetRootComponent())
	{
		return;
	}
	PlayerCharacter->OnLocationReached.Broadcast(LocationName);
	if (UWidgetComponent* const LocMarker = GetLocationMarkerWidgetComp())
	{
		LocMarker->SetHiddenInGame(true);
	}
}

void AQuestMarker::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACyborg* const PlayerCharacter = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter != OtherActor || OtherComp != PlayerCharacter->GetRootComponent())
	{
		return;
	}
	if (UWidgetComponent* const LocMarker = GetLocationMarkerWidgetComp())
	{
		LocMarker->SetHiddenInGame(false);
	}
}

void AQuestMarker::ShowAndActiveAllAttachedActors()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors,true,true);
	for (auto& Actor : AttachedActors)
	{
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorTickEnabled(true);
		Actor->SetActorEnableCollision(true);
	}
}

void AQuestMarker::HideAndDeactiveAllAttachedActors()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors,true,true);
	for (auto& Actor : AttachedActors)
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorTickEnabled(false);
		Actor->SetActorEnableCollision(false);
	}
}

void AQuestMarker::HandleCompletedQuestObjectives()
{
	const TArray<FQuestData>& AllComplitedQuestData = PlayerQuestLog->GetCompletedQuestData();
	for (auto& CompletedQuestData : AllComplitedQuestData)
	{
		for (auto& ComplitedStageData : CompletedQuestData.Stages)
		{
			for (auto& ComplitedObjectiveData : ComplitedStageData.Objectives)
			{
				if (ComplitedObjectiveData.bIsQuestTargetOnly)
				{
					OnQuestObjectiveEnd(ComplitedObjectiveData);
				}
				else
				{
					OnQuestObjectiveBegin(ComplitedObjectiveData, false);
				}
			}
		}
	}
}
