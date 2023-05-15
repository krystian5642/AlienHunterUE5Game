// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LocationMarker.generated.h"

class USphereComponent;
class UWidgetComponent;
class ACharacter;

UCLASS()
class ALIENHUNTER_API ALocationMarker : public AActor
{
	GENERATED_BODY()

	/** if the player character overlaps with this sphere , a location marker disappears */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LocationMarkerWidgetComp;

	UPROPERTY()
	UUserWidget* LocationMarkerWidget;
	
public:	
	// Sets default values for this actor's properties
	ALocationMarker();

	virtual void Tick(float DeltaTime) override;

	void StartTrackingMarker();

	void EndTrackingMarker();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {};

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UPROPERTY()
	ACharacter* Player;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FName LocationName;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	UUserWidget* MapLocationMarker;

private:
	/** true if UserWidget that is used to show distance between this actor and player character implements PlayerLocationInterface */
	bool bCanUpdateLocationOnWidget;

private:
	void InitializeSphereComponent();

	void InitializeLocationMarkerWidgetComp();

public:
	FORCEINLINE USphereComponent* GetSphereComponent() const
	{
		return SphereComponent;
	}

	FORCEINLINE UWidgetComponent* GetLocationMarkerWidgetComp() const
	{
		return LocationMarkerWidgetComp;
	}

	FORCEINLINE UUserWidget* GetLocationMarkerWigdet() const
	{
		return LocationMarkerWidget;
	}


};
