// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestLogComponent.h"
#include "Actors/DifferentActors/QuestTracker.h"
#include "Kismet/GameplayStatics.h"
#include "Structs/QuestData.h"
#include "Actors/DifferentActors/QuestMarker.h"
#include "Engine/DataTable.h"

// Sets default values for this component's properties
UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
    QuestTracerClass = AQuestTracker::StaticClass();
}

bool UQuestLogComponent::StartNewQuest(const FName& QuestID, FQuestData QuestData)
{
    AQuestTracker* const NewQuest = Cast<AQuestTracker>(UGameplayStatics::BeginDeferredActorSpawnFromClass
    (
       this, 
       QuestTracerClass, 
       GetOwner()->GetActorTransform()
    ));
    NewQuest->QuestData = QuestData;
    NewQuest->QuestData.QuestID = QuestID;
    UGameplayStatics::FinishSpawningActor(NewQuest, GetOwner()->GetActorTransform());
    ActiveQuest = NewQuest;
    StartTrackingQuest(NewQuest);
    return true;  
}

void UQuestLogComponent::CompleteQuest(const FQuestData& QuestData)
{
    CompletedQuestData.Add(QuestData);
}

bool UQuestLogComponent::StartTrackingQuest(AQuestTracker* const Quest)
{
    if (!IsValid(Quest) || Quest!=ActiveQuest)
    {
        return false;
    }
    CurrentTrackedQuestID = Quest->QuestData.QuestID;
    Quest->bIsTracked = true;
    OnQuestBeginTracked.Broadcast(Quest);
    return true;
}

bool UQuestLogComponent::EndTrackingQuest(AQuestTracker* const Quest)
{
    if (!IsValid(Quest) || Quest != ActiveQuest)
    {
        return false;
    }
    CurrentTrackedQuestID = FName("");
    Quest->bIsTracked = false;
    OnQuestEndTracked.Broadcast(Quest);
    return true;
}

void UQuestLogComponent::BeginPlay()
{
    Super::BeginPlay();

    // Start zero quest
    if (QuestDataTable)
    {
        const FName ZeroQuestID(TEXT("ax"));
        const FQuestData* const ZeroQuestData = QuestDataTable->FindRow<FQuestData>(ZeroQuestID,FString());
        if (ZeroQuestData)
        {
            StartNewQuest(ZeroQuestID, *ZeroQuestData);
            StartTrackingQuest(ActiveQuest);
        }
    }
}
