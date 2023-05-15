// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{

}

float UHealthComponent::ApplyDamage(float Damage)
{
	Damage = FMath::Clamp(Damage, 0, CurrentHealth);
	CurrentHealth -= Damage;
	if (Damage > 0)
	{
		OnHealthValueChanged.Broadcast(CurrentHealth);
	}
	return Damage;
}

#if WITH_EDITOR
void UHealthComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	SetCurrentHealth(CurrentHealth);
}
#endif

