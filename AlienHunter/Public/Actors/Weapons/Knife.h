// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Knife.generated.h"

/**
 * 
 */
UCLASS()
class ALIENHUNTER_API AKnife : public AWeapon
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* StabComponent;

public:
	AKnife();

	/** Function to activate stab box , probably to apply damage to the enemy using this knife */
	void ActivateStabBox();

	/** Function to deactivate stab box */
	void DeactivateStabBox();

protected:
	virtual void BeginPlay() override;

};
