// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlienHunterGameMode.h"
#include "AlienHunterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Characters/Cyborg.h"
#include "Structs/QuestData.h"

AAlienHunterGameMode::AAlienHunterGameMode()
{

}

void AAlienHunterGameMode::OnCharacterDeath(ACharacter* DeadCharacter)
{
	if (!IsValid(DeadCharacter))
	{
		return;
	}
	ACyborg* const CyborgPlayer = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this,0));
	if (CyborgPlayer && CyborgPlayer == DeadCharacter)
	{
		ShowPlayerDeadScreen();
	}
	else if(CyborgPlayer)
	{
		// DeadCharacter is a cyborg enemy
		FReward Reward(CurrencyRewardForKillingEnemy);
		CyborgPlayer->PresentReward(Reward);
		CyborgPlayer->OnEnemyKilled.Broadcast(DeadCharacter->GetClass());
	}
}
