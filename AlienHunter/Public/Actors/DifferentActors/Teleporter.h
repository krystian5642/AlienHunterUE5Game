// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AlienHunter/Public/Interfaces/InteractableInterface.h"
#include "Teleporter.generated.h"

class UBoxComponent;

UCLASS()
class ALIENHUNTER_API ATeleporter : public AActor
{
	GENERATED_BODY()

	/** OverlapBox is used to trigger overlap event when the player is overlapping with it */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* OverlapBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	/** Spawned when the teleporter has been activated */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Sound Effects", meta = (AllowPrivateAccess = "true"))
	USoundBase* TeleporterSound;

	/** Spawned when the teleporter has just been activated or deactivated if it has been active */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Effects", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* TeleporterParticle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* TeleporterParticleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* TeleporterComputer;
	
public:	
	// Sets default values for this actor's properties
	ATeleporter();

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	bool ActivateTeleporter(const FName& MapName);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void TeleportToLocation(FName MapName);

protected:
	/** true if the teleporter is currently active */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	bool bIsActive;

	/** The time after a new level is streamed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f))
	float ActivationTime;

public:
	/** Returns overlap box */
	FORCEINLINE UBoxComponent* GetOverlapBox() const
	{
		return OverlapBox;
	}

	/** Returns Mesh */
	FORCEINLINE USkeletalMeshComponent* GetMesh() const
	{
		return Mesh;
	}

};
