// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Items/TeleportKey.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Components/BobbingComponent.h"

ATeleportKey::ATeleportKey()
{
	PrimaryActorTick.bCanEverTick = true;
	RotMovComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotMovComponent"));
	BobbingComponent = CreateDefaultSubobject<UBobbingComponent>(TEXT("BobbingComponent"));
}


void ATeleportKey::Use(AAlienHunterCharacter* Character)
{
}
