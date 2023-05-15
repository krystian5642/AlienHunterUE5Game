// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "AlienHunterController.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API AAlienHunterController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	FGenericTeamId TeamID = FGenericTeamId(0);

public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	
};
