// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Items/Medical.h"
#include "Components/HealthComponent.h"
#include "AlienHunter/AlienHunterCharacter.h"


void AMedical::BeginPlay()
{
	Super::BeginPlay();
}

void AMedical::Use(AAlienHunterCharacter* Character)
{
	if (!IsValid(Character))
	{
		return;
	}

	UHealthComponent* const CharHealthComp = Character->GetHealthComponent();

	if (!CharHealthComp)
	{
		return;
	}

	float MaxCharacterHealth = CharHealthComp->GetMaxHealth();
	float HealthToBeAdded = AddHealthPercent * MaxCharacterHealth / 100.0f;

	CharHealthComp->AddHealth(HealthToBeAdded);
}
