// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DifferentActors/LocationMarker.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/PlayerLocationInterface.h"
#include "GameFramework/Character.h"
#include "AlienHunter/Public/Actors/Characters/Cyborg.h"
#include "AlienHunter/Public/Components/QuestLogComponent.h"

// Sets default values
ALocationMarker::ALocationMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitializeSphereComponent();
	InitializeLocationMarkerWidgetComp();

}

void ALocationMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Player && LocationMarkerWidget && bCanUpdateLocationOnWidget)
	{
		// Distance Unit meters
		const int DistanceToPlayer = GetDistanceTo(Player) / 100;
		IPlayerLocationInterface::Execute_GetDistanceToPlayer(LocationMarkerWidget,DistanceToPlayer);
	}
}

void ALocationMarker::StartTrackingMarker()
{
	GetLocationMarkerWidgetComp()->SetHiddenInGame(false);
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	SetActorEnableCollision(true);
}

void ALocationMarker::EndTrackingMarker()
{
	GetLocationMarkerWidgetComp()->SetHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
}

void ALocationMarker::InitializeSphereComponent()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapPawnOnly"));
	SphereComponent->SetGenerateOverlapEvents(true);
}

void ALocationMarker::InitializeLocationMarkerWidgetComp()
{
	LocationMarkerWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LocationMarketWidget"));
	LocationMarkerWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LocationMarkerWidgetComp->SetupAttachment(RootComponent);
	LocationMarkerWidgetComp->SetRelativeLocation(FVector(0, 0, SphereComponent->GetScaledSphereRadius()));

	static ConstructorHelpers::FClassFinder<UUserWidget> LocationMarkerClass(TEXT("/Game/Blueprints/WidgetComponents/W_LocationMarker"));
	if (LocationMarkerClass.Succeeded())
	{
		LocationMarkerWidgetComp->SetWidgetClass(LocationMarkerClass.Class);
	}
}

void ALocationMarker::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this,&ALocationMarker::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ALocationMarker::OnSphereEndOverlap);

	Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	LocationMarkerWidget = LocationMarkerWidgetComp->GetUserWidgetObject();
	if (LocationMarkerWidget)
	{
		bCanUpdateLocationOnWidget = LocationMarkerWidget->GetClass()->ImplementsInterface(UPlayerLocationInterface::StaticClass());
	}

}

void ALocationMarker::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACyborg* const PlayerCharacter = Cast<ACyborg>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter != OtherActor || OtherComp != PlayerCharacter->GetRootComponent())
	{
		return;
	}
	PlayerCharacter->OnLocationReached.Broadcast(LocationName);
	if (MapLocationMarker)
	{
		MapLocationMarker->RemoveFromParent();
	}
	Destroy();
}

void ALocationMarker::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (LocationMarkerWidgetComp)
	{
		LocationMarkerWidgetComp->SetRelativeLocation(FVector(0, 0, 250));
	}
}


