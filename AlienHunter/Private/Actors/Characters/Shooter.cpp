// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/Shooter.h"
#include "Actors/Weapons/Weapon.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Weapons/Gun.h"

AShooter::AShooter()
{
	// Create and attache start trace component
	StartTraceComponent = CreateDefaultSubobject<USceneComponent>(TEXT("StartTraceComponent"));
	StartTraceComponent->SetupAttachment(GetMesh());
}

void AShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateLeftHandTransform();
}

bool AShooter::CanAim() const
{
	const bool bIsFlying = GetCharacterMovement()->IsFlying();
	const bool bIsFalling = GetCharacterMovement()->IsFalling();
	const bool bHasShooterWeapon = IsValid(CurrentUsedWeapon);
	if (bIsFlying || bIsFalling || !bHasShooterWeapon)
	{
		return false;
	}

	// Shooter can always aim if he has a knife
	const bool bHasKnife = CurrentUsedWeapon->ItemData.WeaponType == EWeaponType::Knife;
	if (bHasKnife)
	{
		return true;
	}

	/**
	 * This function uses SweepSingleByChannel to check if there would be collisions with a character weapon if he tried aiming.
	 * It helps us to avoid weapon Magazineping through objects
	*/

	const FVector CameraDirection = GetCurrentCamera()->GetForwardVector();
	const FVector Start = StartTraceComponent->GetComponentLocation() + CameraDirection * 45;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	FQuat Quat(GetCurrentCamera()->GetForwardVector().Rotation() + FRotator(90,10,0));
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(30,50);
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(CurrentUsedWeapon);

	return !GetWorld()->SweepSingleByChannel
	(
		HitResult,
		Start,
		Start,
		Quat,
		ECC_Visibility,
		CollisionShape,
		Params
	);
}

void AShooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Combat action 
	PlayerInputComponent->BindAction("ChangeGunFireMode", IE_Pressed, this, &AShooter::ChangeGunFireMode);
	PlayerInputComponent->BindAction("ReloadGun", IE_Pressed, this, &AShooter::ReloadGun);

	// Combat axis
	PlayerInputComponent->BindAxis("TryToAutoAttack", this, &AShooter::TryToAutoAttack);
}

void AShooter::TryToSingleAttack()
{
	if (IsAttacking())
	{
		return;
	}
	if (IsValid(CurrentUsedWeapon) && bIsAiming)
	{
		const float AttackLenght = CurrentUsedWeapon->WeaponAttack();
		GetWorldTimerManager().SetTimer(AttackDelayTimerHandle, AttackLenght, false);
	}
	else if (!IsValid(CurrentUsedWeapon))
	{
		Super::TryToSingleAttack();
	}
}

void AShooter::TryToAutoAttack(float AxisValue)
{
	AGun* const CurrentGun = Cast<AGun>(CurrentUsedWeapon);
	if (IsValid(CurrentGun) && AxisValue && CurrentGun->GetFireMode() == EFireMode::AutoFire)
	{
		// Multiple single attacks are triggered
		TryToSingleAttack();
	}
}

void AShooter::HandleDeath()
{
	Super::HandleDeath();
	DropWeapon();
}

void AShooter::ChangeGunFireMode()
{
	if (AGun* const CurrentGun = Cast<AGun>(CurrentUsedWeapon))
	{
		CurrentGun->ToggleFireMode();
	}
}

void AShooter::ReloadGun()
{
	if (AGun* const CurrentGun = Cast<AGun>(CurrentUsedWeapon))
	{
		CurrentGun->ReloadMagazine();
	}
}

void AShooter::CalculateLeftHandTransform()
{
	if (!IsValid(CurrentUsedWeapon) || !IsValid(CurrentUsedWeapon->GetMesh()) || CurrentUsedWeapon->ItemData.WeaponType!=EWeaponType::Rifle)
	{
		return;
	}
	USkeletalMeshComponent* const WeaponMesh = CurrentUsedWeapon->GetMesh();
	LeftHandTransform = WeaponMesh->GetSocketTransform(TEXT("LeftHandSocket"));

	FVector Position = LeftHandTransform.GetLocation();
	FRotator Rotation = LeftHandTransform.GetRotation().Rotator();
	GetMesh()->TransformToBoneSpace(TEXT("hand_r"),Position,Rotation,Position,Rotation);
	LeftHandTransform.SetLocation(Position);
	LeftHandTransform.SetRotation(Rotation.Quaternion());
}

void AShooter::DropWeapon()
{
	if (!IsValid(CurrentUsedWeapon))
	{
		return;
	}
	USkeletalMeshComponent* const WeaponMesh = CurrentUsedWeapon->GetMesh();
	WeaponMesh->SetCollisionProfileName(TEXT("DeadWeapon"));
	WeaponMesh->SetSimulatePhysics(true);
	CurrentUsedWeapon->SetLifeSpan(GetLifeSpan());
}

void AShooter::EquipCustomWeapon(AWeapon* const Weapon)
{
	if (Weapon)
	{
		Weapon->Use(this);
	}
}

void AShooter::SetCurrentUsedWeapon(AWeapon* const NewWeapon)
{
	if (CurrentUsedWeapon)
	{
		CurrentUsedWeapon->SetActorHiddenInGame(true);
	}
	CurrentUsedWeapon = NewWeapon;
	if (CurrentUsedWeapon)
	{
		CurrentUsedWeapon->SetActorHiddenInGame(false);
	}
}
