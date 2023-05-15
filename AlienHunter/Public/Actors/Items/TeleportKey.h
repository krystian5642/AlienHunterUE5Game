// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Items/Item.h"
#include "TeleportKey.generated.h"

class URotatingMovementComponent;
class UBobbingComponent;

UCLASS()
class ALIENHUNTER_API ATeleportKey : public AItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Movement", meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotMovComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UBobbingComponent* BobbingComponent;

public:
	// Sets default values for this component's properties
	ATeleportKey();

	virtual void Use(AAlienHunterCharacter* Character) override;

};
