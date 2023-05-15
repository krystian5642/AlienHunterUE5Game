// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/QuestGiver.h"
#include "Components/CapsuleComponent.h"
#include "Components/QuestQiverComponent.h"

AQuestGiver::AQuestGiver()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	QuestQiverComponent = CreateDefaultSubobject<UQuestQiverComponent>(TEXT("QuestQiverComponent"));
}
