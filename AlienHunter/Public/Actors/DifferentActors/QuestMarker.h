// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/DifferentActors/LocationMarker.h"
#include "AlienHunter/Public/Structs/QuestData.h"
#include "QuestMarker.generated.h"

class UQuestLogComponent;
class AQuestTracker;

UCLASS()
class ALIENHUNTER_API AQuestMarker : public ALocationMarker
{
	GENERATED_BODY()

public:
	AQuestMarker();

	/**
	* Called when any quest has started being tracked
	*/
	UFUNCTION()
	void OnQuestBeginTracked(AQuestTracker* const Quest);

	/**
	* Called when any quest has ended being tracked
	*/
	UFUNCTION()
	void OnQuestEndTracked(AQuestTracker* const Quest);

	/**
	* Called when any quest objective has begun. It activetes this quest marker if ObjectiveData.ObjectiveID is the same 
	* as StartAtObjectiveID
	* 
	* @param ObjectiveData The data of the quest which has begun
	* @param bIsTracked true if the objective should be tracked
	*/
	UFUNCTION()
	void OnQuestObjectiveBegin(const FObjectiveData& ObjectiveData,bool bIsTracked);

	/**
	* Called when any quest objective has begun. It destroys this quest marker if ObjectiveData.ObjectiveID is the same
	* as StartAtObjectiveID
	*
	* @param ObjectiveData The data of the quest which has begun
	*/
	UFUNCTION()
	void OnQuestObjectiveEnd(const FObjectiveData& ObjectiveData);

protected:
	virtual void BeginPlay() override;

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void ShowAndActiveAllAttachedActors();

	void HideAndDeactiveAllAttachedActors();

	void HandleCompletedQuestObjectives();

protected:
	UPROPERTY()
	UQuestLogComponent* PlayerQuestLog;

	/**
	* This actor becomes visible after the player has started an objecitve with this ID
	* and if the player ended the objecive with this ID , this actor will be destroyed
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FName StartAtObjectiveID;
	
};
