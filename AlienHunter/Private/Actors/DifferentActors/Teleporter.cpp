// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/Teleporter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/AISense_Hearing.h"
#include "TimerManager.h"
#include "AlienHunter/Public/Actors/DifferentActors/TeleporterComputer.h"
#include "TimerManager.h"

// Sets default values
ATeleporter::ATeleporter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapBox"));
	RootComponent = OverlapBox;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	Mesh->SetupAttachment(OverlapBox);

	TeleporterComputer = CreateDefaultSubobject<UChildActorComponent>(TEXT("TeleporterComputer"));
	TeleporterComputer->SetupAttachment(Mesh);
}

void ATeleporter::BeginPlay()
{
	Super::BeginPlay();
	if (TeleporterParticle)
	{
		TeleporterParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			TeleporterParticle,
			Mesh,
			NAME_None,
			FVector(0.0f),
			FRotator(0.0f),
			FVector(1.0f),
			EAttachLocation::KeepRelativeOffset,
			false
		);

		if (TeleporterParticleComp)
		{
			FTimerHandle DeactivateParticleCompHandle;
			GetWorldTimerManager().SetTimer
			(
				DeactivateParticleCompHandle,
				TeleporterParticleComp,
				&UParticleSystemComponent::Deactivate,
				ActivationTime
			);
		}
		
	}
}

bool ATeleporter::ActivateTeleporter_Implementation(const FName& MapName)
{
	if (bIsActive)
	{
		return false;
	}
	if (TeleporterSound)
	{
		UGameplayStatics::SpawnSoundAttached(TeleporterSound, Mesh);
		const FVector SpawnLocation = GetMesh()->GetComponentLocation();
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), SpawnLocation, 1, GetOwner());
	}
	if (TeleporterParticleComp)
	{
		TeleporterParticleComp->Activate(true);
	}
	bIsActive = true;
	FTimerHandle TeleportToLocHandle;
	FTimerDelegate TeleportToLocDelagate = FTimerDelegate::CreateUObject(this,&ATeleporter::TeleportToLocation,MapName);
	GetWorldTimerManager().SetTimer(TeleportToLocHandle, TeleportToLocDelagate, ActivationTime,false);
	return true;
}