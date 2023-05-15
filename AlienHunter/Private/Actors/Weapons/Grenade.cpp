// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/Grenade.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Actors/Characters/Shooter.h"
#include "Actors/Characters/Cyborg.h"
#include "Components/EquipmentComponent.h"
#include "Perception/AISense_Hearing.h"
#include "DrawDebugHelpers.h"

AGrenade::AGrenade()
{
	ItemData.WeaponType = EWeaponType::Grenade;
	ParentSocket = TEXT("GrenadeSocket");
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->BodyInstance.bLockXRotation = true;
	GetMesh()->BodyInstance.bLockYRotation = true;
	GetMesh()->BodyInstance.bLockZRotation = true;
}

void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AGrenade::WeaponAttack()
{
	return ThrowGrenade();
}

bool AGrenade::PredictGrenadePath(FPredictProjectilePathResult& PredictResult)
{
	if (!IsValid(OwnerAsShooter))
	{
		return false;
	}

	UpdateThrowDirection();

	const FTransform GrenadeSocketTransform = OwnerAsShooter->GetMesh()->GetSocketTransform(ParentSocket);

	// Sets predict params
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = GrenadeSocketTransform.GetLocation();
	PredictParams.LaunchVelocity = ThrowDirection * OwnerAsShooter->GetStrenght();
	PredictParams.ProjectileRadius = 5.0f;
	PredictParams.ActorsToIgnore.Add(this);
	PredictParams.ActorsToIgnore.Add(GetOwner());
	PredictParams.TraceChannel = ECC_GameTraceChannel1; // Weapon
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime = 2.5f;

	return UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
}

void AGrenade::BeginPlay()
{
	Super::BeginPlay();
}

void AGrenade::TriggerSoundAndVisuals()
{
	const FVector SpawnLocation = GetMesh()->GetComponentLocation();
	if (GrenadeSound)
	{
		GrenadeSoundComponent = UGameplayStatics::SpawnSoundAtLocation(this, GrenadeSound, SpawnLocation, FRotator::ZeroRotator);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), SpawnLocation, 1, GetOwner());
	}
	if (GrenadeVisual)
	{
		GrenadeVisualComponent = UGameplayStatics::SpawnEmitterAtLocation(this, GrenadeVisual, SpawnLocation, FRotator::ZeroRotator, FVector(VisualEffectMaxScale));
	}
}

float AGrenade::ThrowGrenade()
{
	float MontageAttackLenght = 0.0f;
	if (!IsValid(OwnerAsShooter) || OwnerAsShooter->IsPlayingAnyMontageOnMesh())
	{
		return MontageAttackLenght;
	}
	MontageAttackLenght = PlayWeaponAttackMontageOnOwner();
	const float ThrowTimePoint = MontageAttackLenght * 0.5852f;
	GetWorldTimerManager().SetTimer(AddImpulseDelayTimer,this,&AGrenade::AddImpulseDelay, ThrowTimePoint, false);

	return MontageAttackLenght;
}

bool AGrenade::GrenadeGroundTrace(FHitResult& HitResult)
{
	UWorld* const World = GetWorld();
	if (!World)
	{
		return false;
	}
	const FVector Start = GetMesh()->Bounds.Origin;
	const FVector End = Start - FVector(0, 0,50);
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(this);

	return World->LineTraceSingleByChannel
	(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);
}

void AGrenade::EnablePhysicsOnMesh()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionObjectType(ECC_GameTraceChannel1); //Weapon
}

void AGrenade::AddImpulseDelay()
{
	if (!IsValid(OwnerAsShooter))
	{
		return;
	}
	SetRootComponent(GetMesh());
	EnablePhysicsOnMesh();
	UpdateThrowDirection(true);
	const FVector StartVelocity = ThrowDirection * OwnerAsShooter->GetStrenght();
	GetMesh()->AddImpulse(StartVelocity, NAME_None, true);
	GetWorldTimerManager().SetTimer(ActivateDelayTimer, this, &AGrenade::Activate, ActivateDelayTime, false);
}

void AGrenade::Activate()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	ApplyDamage();
	TriggerSoundAndVisuals();

	FHitResult HitResult;
	if (GrenadeGroundTrace(HitResult))
	{
		SpawnSurfaceReaction(HitResult);
	}

	Destroy();
}

#if WITH_EDITOR
void AGrenade::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DamageInnerRadius = FMath::Clamp(DamageInnerRadius, 0, DamageOuterRadius);
}
#endif

void AGrenade::CreateItemTextInfo()
{
	Super::CreateItemTextInfo();

	ItemData.ItemTextInfo.Append("\n");
	ItemData.ItemTextInfo.Append("Damage Inner Radius : " + FString::SanitizeFloat(DamageInnerRadius/100.0f)+ "m" + "\n");
	ItemData.ItemTextInfo.Append("Damage Outer Radius : " + FString::SanitizeFloat(DamageOuterRadius/100.0f) + "m");
}

void AGrenade::UpdateThrowDirection(bool bHasThrowingStarted)
{
	if (!IsValid(OwnerAsShooter))
	{
		return;
	}

	// Pitch cannot be changed when the owner has started throwing the greande
	const FRotator CameraRotation = OwnerAsShooter->GetCurrentCamera()->GetComponentRotation();
	const float ProperPitch = bHasThrowingStarted ? ThrowDirection.Rotation().Pitch - AimTargetOffsetAngle : CameraRotation.Pitch;
	const FRotator ProperRotation(ProperPitch,CameraRotation.Yaw,CameraRotation.Roll);

	const FRotator RotateOffset(AimTargetOffsetAngle, 0, 0);
	ThrowDirection = RotateOffset.RotateVector(FVector::UnitX());
	ThrowDirection = ProperRotation.RotateVector(ThrowDirection);

	// Yaw rotation of the owner should be the same as throw yaw rotation to throw the grenade streight
	const float DeltaYaw = OwnerAsShooter->GetActorRotation().Yaw - ThrowDirection.Rotation().Yaw;

	const FRotator DeltaYawRotator(0,DeltaYaw,0);
	ThrowDirection = DeltaYawRotator.RotateVector(ThrowDirection);

}

void AGrenade::ApplyDamage()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamageWithFalloff
	(
		GetWorld(),
		MaxDamage,
		MinDamage,
		GetActorLocation(),
		DamageInnerRadius,
		DamageOuterRadius,
		DamageTypeClass.GetDefaultObject()->DamageFalloff,
		DamageTypeClass,
		IgnoredActors,
		this,
		OwnerAsShooter->GetController()
	);

}