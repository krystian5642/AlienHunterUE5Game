// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MagazineComponent.h"

void UMagazineComponent::ReloadAmmunition()
{
	CurrentAmmoInMagazine = MaxAmmoInMagazine;
	OnCurrentAmmoChange.Broadcast(CurrentAmmoInMagazine);
}

void UMagazineComponent::SubtractBullet()
{
	if (!IsEmpty())
	{
		CurrentAmmoInMagazine--;
		OnCurrentAmmoChange.Broadcast(CurrentAmmoInMagazine);
	}
}