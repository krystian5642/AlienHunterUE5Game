// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BobbingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UBobbingComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Start Location of the owning actor */
	FVector StartLocation;

public:	
	// Sets default values for this component's properties
	UBobbingComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentSpeed() const
	{
		const float Alpha = (MaxSpeed / Amplitude) * GetOwner()->GetGameTimeSinceCreation();
		const float CosAlpha = FMath::Cos(Alpha);
		return MaxSpeed * CosAlpha;
	}

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = 0.0f, ForceUnits = "cm/s"))
	float MaxSpeed = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = 0, ForceUnits = "cm"))
	float Amplitude = 10;
		
};
