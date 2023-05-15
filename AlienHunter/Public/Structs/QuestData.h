#pragma once

#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "AlienHunter/Public/Actors/Items/Item.h"
#include "QuestData.generated.h"


UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	Location,
	Kill,
	ReceiveNewQuest,
	Collect
};

USTRUCT(BlueprintType)
struct FReward
{
	GENERATED_BODY()

	FReward() = default;

	FReward(int CurrencyReward) 
		: CurrencyReward(CurrencyReward) 
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	int CurrencyReward;

	/**
	* The Int value determines Item Count but if and only if an item can't be only one in the player's inventory
	* if this value is less than 0 , it is treated as 0 
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1))
	TMap<TSubclassOf<AItem>, int> ItemRewardClasses;
};

USTRUCT(BlueprintType)
struct FObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ObjectiveID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Type == EObjectiveType::ReceiveNewQuest"))
	FName NextQuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ObjectiveTargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveType Type;

	/** The name of the location that a player must reach to complete the objective */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Type==EObjectiveType::Location"))
	FName LocationName;

	/** Target quantity of actors to kill,pick up or interact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
	int TargetQuantity = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (ClampMin = 0))
	int CurrentQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReward Reward;

	/**
	* true if a target of this objective is visible during a player quest only
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool bIsQuestTargetOnly = true;

};

USTRUCT(BlueprintType)
struct FStageData
{
	GENERATED_BODY()

	/** The name of the stage*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	/** All objectives in this stage*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FObjectiveData> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReward Reward;

};

USTRUCT(BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_BODY()
	
	/** A unique quest name */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(BlueprintReadOnly)
	FName QuestID;

	/** The description of the quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText LogDescription;

	/** All stages of the quest */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStageData> Stages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReward Reward;

};