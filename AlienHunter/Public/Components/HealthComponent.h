// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthValueChanged, float, NewHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALIENHUNTER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (ClampMin = 0.0f,AllowPrivateAccess = "true"))
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f,AllowPrivateAccess = "true"))
	float CurrentHealth = MaxHealth;

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	/**
	* @return Actual damage , cannot be greather than current health
	*/
	float ApplyDamage(float Damage);

	FORCEINLINE void AddHealth(float Health)
	{
		CurrentHealth += Health;
		CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
		OnHealthValueChanged.Broadcast(CurrentHealth);
	}

	FORCEINLINE bool IsAlive() const
	{
		return CurrentHealth > 0;
	}

public:
	UPROPERTY(BlueprintAssignable)
	FOnHealthValueChanged OnHealthValueChanged;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	FORCEINLINE float GetMaxHealth() const 
	{
		return MaxHealth; 
	}
	FORCEINLINE void SetMaxHealth(float NewHealth)
	{
		MaxHealth = FMath::Max(0,NewHealth);
	}

	FORCEINLINE float GetCurrentHealth() const 
	{
		return CurrentHealth; 
	}

	void SetCurrentHealth(float NewCurrentHealth)
	{
		CurrentHealth = FMath::Clamp(CurrentHealth, 0, MaxHealth);
	}
		
};
