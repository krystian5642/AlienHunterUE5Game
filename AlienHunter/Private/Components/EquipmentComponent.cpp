// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EquipmentComponent.h"
#include "Actors/Characters/Cyborg.h"
#include "AlienHunter/Public/Actors/Weapons/Weapon.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{

}

void UEquipmentComponent::EquipWeapon(TSubclassOf<AItem> WeaponClass, int StackCount)
{
	AWeapon* const Weapon = CreateWeapon(WeaponClass, StackCount);
	if (!Weapon)
	{
		return;
	}

	Weapon->GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	Weapon->GetMesh()->SetSimulatePhysics(false);
	Weapon->SetActorHiddenInGame(true);

	// Check if there is a weapon in the equipment of the same type as weapon we want to add
	const EWeaponType WeaponType = Weapon->ItemData.WeaponType;
	const bool bIsSlotTaken = MappedWeaponEquipment.Contains(WeaponType);

	// Remove a weapon if slot is already taken
	if (bIsSlotTaken)
	{
		RemoveWeapon(WeaponType);
	}
	AShooter* const Shooter = Cast<AShooter>(GetOwner());
	if (Shooter)
	{
		// Attaches the weapon mesh to the character mesh
		Weapon->GetMesh()->AttachToComponent
		(
			Shooter->GetMesh(),
			FAttachmentTransformRules::KeepRelativeTransform,
			Weapon->GetParentSocket()
		);
	}

	// Adds the weapon to mapped equipemnt
	MappedWeaponEquipment.Add(WeaponType, Weapon);
	SetCurrentUsedWeaponType(Weapon->ItemData.WeaponType);

	OnEquipmentChanged.Broadcast();
}

void UEquipmentComponent::RemoveWeapon(EWeaponType WeaponType,bool bShouldBeDestroyed)
{
	// Remove the weapon from the mapped weapon equipment
	AWeapon* Weapon = nullptr;
	MappedWeaponEquipment.RemoveAndCopyValue(WeaponType,Weapon);

	if (!Weapon)
	{
		return;
	}

	if (Weapon->ItemData.StackCount > 1 && !bShouldBeDestroyed)
	{
		if (--Weapon->ItemData.StackCount>0)
		{
			EquipWeapon(Weapon->ItemData.ItemClass, Weapon->ItemData.StackCount);
		}
	}
	if (bShouldBeDestroyed)
	{
		Weapon->Destroy();
	}

	OnEquipmentChanged.Broadcast();
}

bool UEquipmentComponent::IsWeaponEquiped(const AWeapon* Weapon) const
{
	if (!IsValid(Weapon))
	{
		return false;
	}
	return MappedWeaponEquipment.FindRef(Weapon->ItemData.WeaponType) == Weapon;
}

bool UEquipmentComponent::IsWeaponClassEquiped(TSubclassOf<AItem> WeaponClass) const
{
	for (const auto& [WeaponType, Weapon] : MappedWeaponEquipment)
	{
		if (Weapon && Weapon->ItemData.ItemClass == WeaponClass)
		{
			return true;
		}
	}
	return false;
}

void UEquipmentComponent::SetCurrentUsedWeaponType(EWeaponType WeaponType)
{
	AWeapon* const Weapon = GetWeaponOfType(WeaponType);
	AShooter* const OwnerShooter = Cast<AShooter>(GetOwner());
	if (!Weapon)
	{
		CurrentUsedWeaponType = EWeaponType::Unknown;
		if (OwnerShooter)
		{
			OwnerShooter->SetCurrentUsedWeapon(nullptr);
		}
		OnWeaponUsed.Broadcast(CurrentUsedWeaponType);
		return;
	}
	CurrentUsedWeaponType = WeaponType;
	Weapon->Use(OwnerShooter);
	OnWeaponUsed.Broadcast(CurrentUsedWeaponType);
}

AWeapon* UEquipmentComponent::CreateWeapon(TSubclassOf<AItem> WeaponClass, int StackCount)
{
	if (!WeaponClass || StackCount <= 0)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnWeaponParameters;
	SpawnWeaponParameters.Owner = GetOwner();

	// Spawns a proper weapon from WeaponClass 
	AWeapon* const Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnWeaponParameters);
	if (Weapon)
	{
		Weapon->ItemData.StackCount = StackCount;
	}
	return Weapon;
}

