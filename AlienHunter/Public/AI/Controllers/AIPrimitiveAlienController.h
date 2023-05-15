// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIPrimitiveAlienController.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Touch;
class UAISenseConfig_Prediction;
struct FAIStimulus;

UCLASS()
class ALIENHUNTER_API AAIPrimitiveAlienController : public AAIController
{
	GENERATED_BODY()

	FGenericTeamId TeamID = FGenericTeamId(1);

public:
	AAIPrimitiveAlienController(const FObjectInitializer& ObjectInitializer);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BaseBehaviourTree;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY()
	UAISenseConfig_Touch* TouchConfig;

	UPROPERTY()
	UAISenseConfig_Prediction* PredictionConfig;

private:
	/** Initialize sight config , called in the constructor */
	void InitializeSightConfig();

	/** Initialize hearing config , called in the constructor */
	void InitializeHearingConfig();

	/** Initialize touch config , called in the constructor */
	void InitializeTouchConfig();

	/** Initialize prediction config , called in the constructor */
	void InitializePredictionConfig();

	UFUNCTION()
	void OnUpdated(const TArray<AActor*>& UpdatedActors);

	/** Called when the controlled actor is hit/touched */
	UFUNCTION()
	void OnControlledPawnTouched(AActor* ControlledPawn, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
	
};
