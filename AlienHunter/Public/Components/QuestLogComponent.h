// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlienHunter/Public/Structs/QuestData.h"
#include "QuestLogComponent.generated.h"

class AQuestTracker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestBegin, AQuestTracker*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestBeginTracked, AQuestTracker*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestEndTracked, AQuestTracker*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStageBegin, const FStageData&, StageData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestObjectiveBegin, const FObjectiveData&, ObjectiveData,bool, bIsTracked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestEnd, const FQuestData&, QuestData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStageEnd, const FStageData&, StageData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestObjectiveEnd, const FObjectiveData&, ObjectiveData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestLogComponent();

	/**
	* 
	* @param QuestID The unique ID which will be assigned to the new created quest
	* @param QuestData The Data about the new quest, it will be assigned to the new quest too
	* 
	* @return true if the quest was successfuly created
	*/
	UFUNCTION(BlueprintCallable)
	bool StartNewQuest(const FName& QuestID, FQuestData QuestData);

	/** Adds a quest data to CompletedQuests array */
	void CompleteQuest(const FQuestData& QuestData);

	/**
	* @param Quest The quest to be tracked
	* 
	* @return true if the quest has started being tracked
	*/
	UFUNCTION(BlueprintCallable)
	bool StartTrackingQuest(AQuestTracker* const Quest);

	/**
	* @param Quest The quest to be tracked
	* 
	* @return true if the quest has ended being tracked
	*/
	UFUNCTION(BlueprintCallable)
	bool EndTrackingQuest(AQuestTracker* const Quest);

public:
	/** Called when a new quest begins */
	UPROPERTY(BlueprintAssignable)
	FOnQuestBegin OnQuestBegin;

	/** Called when a new quest stage begins */
	UPROPERTY(BlueprintAssignable)
	FOnQuestStageBegin OnQuestStageBegin;

	/** Called when a new quest objecitve begins */
	UPROPERTY(BlueprintAssignable)
	FOnQuestObjectiveBegin OnQuestObjectiveBegin;

	/** Called when a quest ends */
	UPROPERTY(BlueprintAssignable)
	FOnQuestEnd OnQuestEnd;

	/** Called when a quest stage ends */
	UPROPERTY(BlueprintAssignable)
	FOnQuestStageEnd OnQuestStageEnd;

	/** Called when a quest objective ends*/
	UPROPERTY(BlueprintAssignable)
	FOnQuestObjectiveEnd OnQuestObjectiveEnd;

	/** Called when a quest has started being tracked */
	UPROPERTY(BlueprintAssignable)
	FOnQuestBeginTracked OnQuestBeginTracked;

	/** Called when a quest has ended being tracked */
	UPROPERTY(BlueprintAssignable)
	FOnQuestEndTracked OnQuestEndTracked;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quests")
	FName CurrentTrackedQuestID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quests")
	AQuestTracker* ActiveQuest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quests")
	TArray<FQuestData> CompletedQuestData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quests")
	TSubclassOf<AQuestTracker> QuestTracerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quests")
	UDataTable* QuestDataTable;

public:
	FORCEINLINE const FName& GetTrackedQuestID() const
	{
		return CurrentTrackedQuestID;
	}
		
	FORCEINLINE AQuestTracker* GetActiveQuest() const
	{
		return ActiveQuest;
	}

	FORCEINLINE const TArray<FQuestData>& GetCompletedQuestData() const
	{
		return CompletedQuestData;
	}
};
