// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/AIPrimitiveAlienController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Touch.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Touch.h"
#include "AlienHunter/AlienHunterCharacter.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "Perception/AISense_Prediction.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "NavigationSystem.h"
#include "Blueprint/UserWidget.h"

AAIPrimitiveAlienController::AAIPrimitiveAlienController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Com"));
	SetPerceptionComponent(*PerceptionComp);
	InitializeSightConfig();
	InitializeHearingConfig();
	InitializeTouchConfig();
	InitializePredictionConfig();
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAIPrimitiveAlienController::OnUpdated);

	Tags.Add(TEXT("AlienTeam"));
}

ETeamAttitude::Type AAIPrimitiveAlienController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (const APawn* const OtherPawn = Cast<const APawn>(&Other))
	{
		if (const auto TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
		{
			return TeamAgent->GetGenericTeamId() == GetGenericTeamId() ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
		}
	}
	return ETeamAttitude::Neutral;
}

FGenericTeamId AAIPrimitiveAlienController::GetGenericTeamId() const
{
	return TeamID;
}

void AAIPrimitiveAlienController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!IsValid(InPawn))
	{
		return;
	}
	InPawn->OnActorHit.AddDynamic(this, &AAIPrimitiveAlienController::OnControlledPawnTouched);
	if (BaseBehaviourTree)
	{
		RunBehaviorTree(BaseBehaviourTree);

		// Set Start Location of the controlled pawn
		UBlackboardComponent* const BlackboardComp = GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector("StartLocation", InPawn->GetActorLocation());
		}
	}
}

void AAIPrimitiveAlienController::InitializeSightConfig()
{
	// Create sight config
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		// Initialise sight config 
		SightConfig->SightRadius = 5000.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 500.0f;
		SightConfig->PeripheralVisionAngleDegrees = 100.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 500.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

		// Add to percepion component and set as a dominant sense
		GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void AAIPrimitiveAlienController::InitializeHearingConfig()
{
	// Create hearing config
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));

	if (HearingConfig)
	{
		// Initialise sight config 
		HearingConfig->HearingRange = 10000.0f;
		HearingConfig->SetMaxAge(5.0f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;

		// Add to percepion component
		GetPerceptionComponent()->ConfigureSense(*HearingConfig);
	}
}

void AAIPrimitiveAlienController::InitializeTouchConfig()
{
	// Create touch config
	TouchConfig = CreateDefaultSubobject<UAISenseConfig_Touch>(TEXT("TouchConfig"));

	if (TouchConfig)
	{
		// Initialise sight config 
		TouchConfig->SetMaxAge(0.8f);

		// Add to percepion component
		GetPerceptionComponent()->ConfigureSense(*TouchConfig);
	}
}

void AAIPrimitiveAlienController::InitializePredictionConfig()
{
	// Create prediction config
	PredictionConfig = CreateDefaultSubobject<UAISenseConfig_Prediction>(TEXT("PredictionConfig"));

	if (PredictionConfig)
	{
		PredictionConfig->SetMaxAge(5.0f);

		// Add to percepion component
		GetPerceptionComponent()->ConfigureSense(*PredictionConfig);
	}
}

void AAIPrimitiveAlienController::OnUpdated(const TArray<AActor*>& UpdatedActors)
{
	UBlackboardComponent* const BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}
	for (AActor* const UpdatedActor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		GetPerceptionComponent()->GetActorsPerception(UpdatedActor,Info);
		for (const FAIStimulus& AIStimulus : Info.LastSensedStimuli)
		{
			FName StimulusTypeName = AIStimulus.Type.Name;
			if (StimulusTypeName == "Default__AISense_Sight")
			{
				bool HasPlayerBeingSeen = Blackboard->GetValueAsBool("CanSeePlayer");
				BlackboardComp->SetValueAsBool("CanSeePlayer", AIStimulus.WasSuccessfullySensed());
				if (AIStimulus.WasSuccessfullySensed())
				{
					return;
				}
				bool CanSeePlayerNow = Blackboard->GetValueAsBool("CanSeePlayer");

				if (HasPlayerBeingSeen != CanSeePlayerNow && !CanSeePlayerNow)
				{
					BlackboardComp->SetValueAsBool("ShouldInvestigate", true);
					ACharacter* const PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
					UAISense_Prediction::RequestPawnPredictionEvent(GetPawn(), PlayerCharacter, 1.0f);
				}
			}
			else if (StimulusTypeName == "Default__AISense_Hearing" && AIStimulus.WasSuccessfullySensed())
			{
				FVector Location = AIStimulus.StimulusLocation;
				ANavigationData* const NavData = UNavigationSystemV1::GetCurrent(GetWorld())->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
				FNavLocation NavLoc;
				if (NavData)
				{
					NavData->ProjectPoint(Location, NavLoc, FVector(1000));
					Location = NavLoc.Location;

					BlackboardComp->SetValueAsVector("TargetLocation", Location);
					BlackboardComp->SetValueAsBool("ShouldInvestigate", AIStimulus.WasSuccessfullySensed());
				}

			}
			else if (StimulusTypeName == "Default__AISense_Touch" && AIStimulus.WasSuccessfullySensed())
			{
				BlackboardComp->SetValueAsBool("WasTouched", AIStimulus.WasSuccessfullySensed());
				BlackboardComp->SetValueAsVector("TouchLocation", AIStimulus.StimulusLocation);
			}
			else if (StimulusTypeName == "Default__AISense_Prediction" && AIStimulus.WasSuccessfullySensed())
			{
				FVector Location = AIStimulus.StimulusLocation;
				ANavigationData* const NavData = UNavigationSystemV1::GetCurrent(GetWorld())->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
				if (NavData)
				{
					FNavLocation NavLoc;
					NavData->ProjectPoint(Location, NavLoc, FVector(1000));
					Location = NavLoc.Location;
					BlackboardComp->SetValueAsVector("TargetLocation", Location);
				}
			}
		}
	}
}

void AAIPrimitiveAlienController::OnControlledPawnTouched(AActor* ControlledPawn, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	AAlienHunterCharacter* const AlienHunterChar = Cast<AAlienHunterCharacter>(OtherActor);
	UAIPerceptionSystem* const PerceptionSystem = UAIPerceptionSystem::GetCurrent(OtherActor);
	const bool bIsOtherActorPlayer = OtherActor == UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PerceptionSystem || !AlienHunterChar || ControlledPawn == OtherActor || !bIsOtherActorPlayer)
	{
		return;
	}
	const FVector EventLocation(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, Hit.ImpactPoint.Z);
	const FAITouchEvent TouchEvent = FAITouchEvent(ControlledPawn, OtherActor, EventLocation);
	PerceptionSystem->OnEvent(TouchEvent);
}
