// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlienHunter/Public/Structs/QuestData.h"
#include "QuestTracker.generated.h"

class UQuestLogComponent;
class AQuestMarker;

UCLASS()
class ALIENHUNTER_API AQuestTracker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestTracker();

	/**
	* Updates a objective if and only if it's not the location objective
	* 
	* @param ActorClass The subclassof actor which the player interacted, killed
	*/
	UFUNCTION()
	void UpdateQuantityObjective(TSubclassOf<AActor> ActorClass);

	/**
	* Updates a objective if and only if it's the location objective
	* 
	* @param LocationName The name of the location which the player reached
	*/
	UFUNCTION()
	void UpdateLocations(const FName& LocationName);

	UFUNCTION()
	void OnPlayerReceiveNewQuest(AQuestTracker* NewQuest);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestInfo")
	FQuestData QuestData;

	/** true if a quest is being tracked by the player */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bIsTracked;

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo")
	FStageData CurrentStageData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuestInfo")
	FObjectiveData CurrentObjectiveData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestInfo", meta = (ClampMin = 0))
	int CurrentStageNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestInfo", meta = (ClampMin = 0))
	int	CurrentObjectiveNum;

	UPROPERTY()
	UQuestLogComponent* PlayerQuestLogComp;

	UPROPERTY()
	ACyborg* CyborgPlayer;

private:
	void InternalCompleteObjective();

	/**
	* @return true if a stage has been Completed
	*/
	bool InternalUpdateStage();

	/**
	* @return true if a quest has been Completed
	*/
	bool InternalUpdateQuest(const bool bHasStageCompleted);

public:
	FORCEINLINE const FStageData& GetCurrentStageData() const
	{
		return CurrentStageData;
	}

	FORCEINLINE const FObjectiveData& GetCurrentObjectiveData() const
	{
		return CurrentObjectiveData;
	}

	FORCEINLINE int GetCurrentStageNum() const
	{
		return CurrentStageNum;
	}

	FORCEINLINE int GetCurrentObjectiveNum() const
	{
		return CurrentObjectiveNum;
	}
};
