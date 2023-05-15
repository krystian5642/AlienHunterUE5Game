// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/Item.h"
#include "Medical.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API AMedical : public AItem
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

protected:
	/** Sets default value for AddHealthPercent*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nutritiousness", meta = (ClampMin = 0, ClampMax = 100))
	float AddHealthPercent = 5;

public:
	virtual void Use(AAlienHunterCharacter* Character) override;

public:
	FORCEINLINE float GetAddHealthPercent() const 
	{ 
		return AddHealthPercent; 
	}

	FORCEINLINE void SetAddHealthPercent(float NewAddHealthPercent)
	{
		AddHealthPercent = FMath::Clamp(NewAddHealthPercent,0,100);
	}

	
};
