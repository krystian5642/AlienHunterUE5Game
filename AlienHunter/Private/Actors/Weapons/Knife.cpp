// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/Knife.h"
#include "Actors/Characters/Shooter.h"

AKnife::AKnife()
{
	ParentSocket = TEXT("KnifeSocket");
	StabComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("StabComponent"));
	StabComponent->SetupAttachment(GetMesh());
	DeactivateStabBox();
}

void AKnife::ActivateStabBox()
{
	if (!StabComponent)
	{
		return;
	}
	
	StabComponent->SetCollisionProfileName(TEXT("CloseCombat"));
	StabComponent->SetGenerateOverlapEvents(true);
	StabComponent->Activate();
}

void AKnife::DeactivateStabBox()
{
	if (!StabComponent)
	{
		return;
	}

	StabComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StabComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	StabComponent->SetGenerateOverlapEvents(false);
	StabComponent->Deactivate();
}

void AKnife::BeginPlay()
{
	Super::BeginPlay();
	if (StabComponent && OwnerAsShooter)
	{
		StabComponent->OnComponentBeginOverlap.AddDynamic(OwnerAsShooter, &AShooter::ApplyMeleeDamage);
	}
}
