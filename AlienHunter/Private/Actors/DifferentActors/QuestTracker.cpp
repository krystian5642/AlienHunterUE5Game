// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/QuestTracker.h"
#include "Actors/Characters/Cyborg.h"
#include "Components/QuestLogComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/DifferentActors/QuestMarker.h"

// Sets default values
AQuestTracker::AQuestTracker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AQuestTracker::UpdateQuantityObjective(TSubclassOf<AActor> ActorClass)
{
    if (CurrentObjectiveData.Type == EObjectiveType::Location || CurrentObjectiveData.Type == EObjectiveType::ReceiveNewQuest)
    {
        return;
    }
    bool bHasObjectiveCompleted = false;

    const TSubclassOf<AActor>& QuestTargetClass = CurrentObjectiveData.ObjectiveTargetClass;
    if (QuestTargetClass == ActorClass)
    {
        int& CurrentQuantity = CurrentObjectiveData.CurrentQuantity;
        CurrentQuantity++;
        bHasObjectiveCompleted = (CurrentQuantity == CurrentObjectiveData.TargetQuantity);
    }

    if (bHasObjectiveCompleted)
    {
        InternalCompleteObjective();
    }
}

void AQuestTracker::UpdateLocations(const FName& LocationName)
{
    if (LocationName.IsNone() || CurrentObjectiveData.Type!=EObjectiveType::Location)
    {
        return;
    }
    bool bHasObjectiveCompleted = CurrentObjectiveData.LocationName.IsEqual(LocationName);

    if (bHasObjectiveCompleted)
    {
        InternalCompleteObjective();
    }
}

void AQuestTracker::OnPlayerReceiveNewQuest(AQuestTracker* NewQuest)
{
    EObjectiveType CurrentObjectiveType = CurrentObjectiveData.Type;
    if (CurrentObjectiveType != EObjectiveType::ReceiveNewQuest || !IsValid(NewQuest))
    {
        return;
    }

    bool bHasObjectiveCompleted = false;
    
    if (NewQuest->QuestData.QuestID.IsEqual(CurrentObjectiveData.NextQuestID))
    {
        bHasObjectiveCompleted = true;
    }

    if (bHasObjectiveCompleted)
    {
        InternalCompleteObjective();
    }
}

void AQuestTracker::BeginPlay()
{
    Super::BeginPlay();
    CyborgPlayer = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (CyborgPlayer)
    {
        PlayerQuestLogComp = CyborgPlayer->GetQuestLogComponent();
        CurrentStageData = QuestData.Stages[CurrentStageNum];
        CurrentObjectiveData = CurrentStageData.Objectives[CurrentStageNum];
        if (PlayerQuestLogComp)
        {
            PlayerQuestLogComp->OnQuestBegin.Broadcast(this);
            PlayerQuestLogComp->OnQuestObjectiveBegin.Broadcast(CurrentObjectiveData,bIsTracked);
            PlayerQuestLogComp->OnQuestStageBegin.Broadcast(CurrentStageData);

            PlayerQuestLogComp->OnQuestBegin.AddDynamic(this, &AQuestTracker::OnPlayerReceiveNewQuest);
        }
        CyborgPlayer->OnItemCollected.AddDynamic(this, &AQuestTracker::UpdateQuantityObjective);
        CyborgPlayer->OnEnemyKilled.AddDynamic(this, &AQuestTracker::UpdateQuantityObjective);
        CyborgPlayer->OnLocationReached.AddDynamic(this,&AQuestTracker::UpdateLocations);
    }
}

void AQuestTracker::InternalCompleteObjective()
{
    if (CyborgPlayer)
    {
        CyborgPlayer->PresentReward(CurrentObjectiveData.Reward);
    }
    PlayerQuestLogComp->OnQuestObjectiveEnd.Broadcast(CurrentObjectiveData);
    CurrentObjectiveNum++;
    const bool bHasStageCompleted = InternalUpdateStage();
    InternalUpdateQuest(bHasStageCompleted);
}

bool AQuestTracker::InternalUpdateStage()
{
    bool bHasStageCompleted = !CurrentStageData.Objectives.IsValidIndex(CurrentObjectiveNum);
    if (bHasStageCompleted)
    {
       if (CyborgPlayer)
       {
            CyborgPlayer->PresentReward(CurrentStageData.Reward);
       }
       PlayerQuestLogComp->OnQuestStageEnd.Broadcast(QuestData.Stages[CurrentStageNum]);
       CurrentStageNum++;
       CurrentObjectiveNum = 0;
    }
    else
    {
       CurrentObjectiveData = CurrentStageData.Objectives[CurrentObjectiveNum];
       PlayerQuestLogComp->OnQuestObjectiveBegin.Broadcast(CurrentObjectiveData,bIsTracked);
    }
    return bHasStageCompleted;
}

bool AQuestTracker::InternalUpdateQuest(const bool bHasStageCompleted)
{
    bool bHasQuestCompleted = false;
    if (bHasStageCompleted)
    {
        bHasQuestCompleted = !QuestData.Stages.IsValidIndex(CurrentStageNum);
        if (bHasQuestCompleted)
        {
            if (CyborgPlayer)
            {
                CyborgPlayer->PresentReward(QuestData.Reward);
            }
            PlayerQuestLogComp->CompleteQuest(QuestData);
            PlayerQuestLogComp->OnQuestEnd.Broadcast(QuestData);
            if (bIsTracked)
            {
                PlayerQuestLogComp->EndTrackingQuest(this);
            }
            Destroy();
        }
        else
        {
            CurrentStageData = QuestData.Stages[CurrentStageNum];
            CurrentObjectiveData = CurrentStageData.Objectives[CurrentObjectiveNum];
            PlayerQuestLogComp->OnQuestStageBegin.Broadcast(CurrentStageData);
            PlayerQuestLogComp->OnQuestObjectiveBegin.Broadcast(CurrentObjectiveData,bIsTracked);
        }
    }
    return bHasQuestCompleted;
}