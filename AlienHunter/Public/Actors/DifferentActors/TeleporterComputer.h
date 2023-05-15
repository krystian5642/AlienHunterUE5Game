// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlienHunter/Public/Interfaces/InteractableInterface.h"
#include "TeleporterComputer.generated.h"

UCLASS()
class ALIENHUNTER_API ATeleporterComputer : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;
	
public:	
	// Sets default values for this actor's properties
	ATeleporterComputer();



};
