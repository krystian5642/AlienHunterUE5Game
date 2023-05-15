// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AlienHunterGameMode.generated.h"

UCLASS(minimalapi)
class AAlienHunterGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAlienHunterGameMode();

	virtual void OnCharacterDeath(ACharacter* DeadCharacter);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowPlayerDeadScreen();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrencyRewardForKillingEnemy = 50;
};



