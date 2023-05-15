// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlienHunter/Public/Structs/QuestData.h"
#include "QuestQiverComponent.generated.h"

class UDataTable;
class UQuestLogComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UQuestQiverComponent : public UActorComponent
{
	GENERATED_BODY()

	/** All quests in the game */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "QuestInfo", meta = (AllowPrivateAccess = "true"))
	UDataTable* QuestsDataTable;

public:	
	// Sets default values for this component's properties
	UQuestQiverComponent();

	UFUNCTION(BlueprintCallable)
	FQuestData GetOfferedQuestData() const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnQuestEnd(const FQuestData& QuestData);

protected:
	UPROPERTY()
	UQuestLogComponent* PlayerQuestLogComp;

	UPROPERTY(BlueprintReadOnly)
	FName OfferedQuestID;

	uint32 CompletedQuestNum;
		
};
