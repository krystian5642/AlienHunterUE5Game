// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/QuestQiverComponent.h"
#include "Engine/DataTable.h"
#include "Actors/Characters/Cyborg.h"
#include "Components/QuestLogComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/DifferentActors/QuestTracker.h"

// Sets default values for this component's properties
UQuestQiverComponent::UQuestQiverComponent()
{
	PrimaryComponentTick.bCanEverTick =false;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/Miscellaneous2/Quests"));
	if (DataTable.Succeeded())
	{
		QuestsDataTable = DataTable.Object;
	}
}

FQuestData UQuestQiverComponent::GetOfferedQuestData() const
{
	if (!QuestsDataTable)
	{
		return FQuestData();
	}
	const FQuestData* const OfferedQuestData = QuestsDataTable->FindRow<FQuestData>(OfferedQuestID,FString());
	return OfferedQuestData ? *OfferedQuestData : FQuestData();
}

void UQuestQiverComponent::BeginPlay()
{
	Super::BeginPlay();
  
    if (ACyborg* const CyborgPlayer = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this, 0)))
    {
        PlayerQuestLogComp = CyborgPlayer->GetQuestLogComponent();
		if (PlayerQuestLogComp)
		{
			CompletedQuestNum = PlayerQuestLogComp->GetCompletedQuestData().Num();
			const uint32 QuestNum = (CompletedQuestNum == 0 || CompletedQuestNum == 1) ? 2 : CompletedQuestNum+1;
			OfferedQuestID = FName(*(FString::FromInt(QuestNum) + FString(TEXT("ax"))));
			PlayerQuestLogComp->OnQuestEnd.AddDynamic(this,&UQuestQiverComponent::OnQuestEnd);
		}
		
    }
}

void UQuestQiverComponent::OnQuestEnd(const FQuestData& QuestData)
{
	if (OfferedQuestID.IsEqual(QuestData.QuestID))
	{
		CompletedQuestNum++;
		const uint32 QuestNum = (CompletedQuestNum == 0 || CompletedQuestNum == 1) ? 2 : CompletedQuestNum+1;
		OfferedQuestID = FName(*(FString::FromInt(QuestNum) + FString(TEXT("ax"))));
	}
}
