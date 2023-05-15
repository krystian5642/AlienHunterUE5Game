// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlienHunter/Public/Interfaces/InteractableInterface.h"
#include "QuestGiver.generated.h"
 
class UCapsuleComponent;
class UQuestQiverComponent;

UCLASS()
class ALIENHUNTER_API AQuestGiver : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	/** The capsule component is set as a root component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* CapsuleComponent;

	/** Mesh is used to graphical representation of the quest giver */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

public:
	AQuestGiver();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UQuestQiverComponent* QuestQiverComponent;
};
