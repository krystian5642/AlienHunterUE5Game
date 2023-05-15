// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/Gun.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Characters/Cyborg.h"
#include "Components/MagazineComponent.h"
#include "Perception/AISense_Hearing.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"

AGun::AGun()
{
	// Sets default values for item data
	ItemData.WeaponType = EWeaponType::Pistol;

	// Sets parent socket
	ParentSocket = "GunSocket";

	MagazineComponent = CreateDefaultSubobject<UMagazineComponent>(TEXT("MagazineComponent"));
	BulletTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BulletTrail"));
	BulletTrail->SetupAttachment(GetMesh(),TEXT("Muzzle"));
	BulletTrail->bAutoActivate = false;
}

float AGun::WeaponAttack()
{
	float AttackLenght = 0.0f;
	if (IsFireDelay() || IsReloading())
	{
		return AttackLenght;
	}

	if (MagazineComponent->IsEmpty())
	{
		ReloadMagazine();
		return AttackLenght;
	}

	TriggerSoundAndVisuals();

	// Subtracts one bullet
	MagazineComponent->SubtractBullet();

	ApplyDamage();

	// Plays the weapon attack montage on the owner shooter
	PlayWeaponAttackMontageOnOwner();

	GetWorldTimerManager().SetTimer(HandleFireDelay, FireRate, false);

	if (MagazineComponent->IsEmpty())
	{
		ReloadMagazine();
	}

	return FireRate;
}


void AGun::ReloadMagazine()
{
	MagazineComponent->ReloadAmmunition();
	if (ReloadSound)
	{
		const FVector SpawnLocation = GetMesh()->GetComponentLocation();
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ReloadSound, SpawnLocation);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), SpawnLocation, 1, GetOwner()->GetInstigatorController());
	}
	float ReloadTime = MagazineComponent->GetMagazineReloadTime();
	GetWorldTimerManager().SetTimer(HandleReloadTimer, ReloadTime, false);
}

bool AGun::ToggleFireMode()
{
	bool bWasFireModeChanged = false;
	if (bCanToggleFireMode)
	{
		switch (FireMode)
		{
		case EFireMode::SingleFire:
			FireMode = EFireMode::AutoFire;
			break;

		case EFireMode::AutoFire:
			FireMode = EFireMode::SingleFire;
			break;

		default:
			break;
		}
		bWasFireModeChanged = true;
	}
	return bWasFireModeChanged;
}

void AGun::TriggerSoundAndVisuals()
{
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GetMesh(), TEXT("Muzzle"));
	}
	if (ShootSound)
	{
		const FVector SpawnSoundLocation = GetMesh()->GetComponentLocation();
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShootSound, SpawnSoundLocation);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), SpawnSoundLocation,1,GetOwner()->GetInstigatorController());
	}
	if (BulletTrail)
	{
		TriggerBulletTrailEffect();
	}
}

void AGun::CreateItemTextInfo()
{
	Super::CreateItemTextInfo();

	ItemData.ItemTextInfo.Append("\n");
	FString CanToggleFireMode = bCanToggleFireMode ? TEXT("Yes") : TEXT("No");
	ItemData.ItemTextInfo.Append("Fire Rate : " + FString::SanitizeFloat(FireRate)+"s" + "\n");
	ItemData.ItemTextInfo.Append("Toggle Fire Mode : " + CanToggleFireMode + "\n");
	ItemData.ItemTextInfo.Append("Fire Range : " + FString::SanitizeFloat(FireRange/100.0f) + "m");
}

void AGun::ApplyDamage()
{
	FVector ShootDirection;
	FVector EndPoint;
	FHitResult HitResult;
	const bool bWasHitSomething = GunTrace(ShootDirection, EndPoint, HitResult);
	LastHitLocation = bWasHitSomething ? HitResult.ImpactPoint : EndPoint;

	if (bWasHitSomething)
	{
		AActor* const HitActor = HitResult.GetActor();
		UPrimitiveComponent* const HitComponent = HitResult.GetComponent();
		float Damage = FMath::RandRange(MinDamage, MaxDamage);

		// Headshot causes immediate death
		if (HitResult.BoneName==TEXT("head"))
		{
			Damage = 1000000000.0f;
		}

		UGameplayStatics::ApplyPointDamage
		(
			HitActor,
			Damage,
			ShootDirection,
			HitResult,
			OwnerAsShooter->GetController(),
			this,
			DamageTypeClass
		);

		SpawnSurfaceReaction(HitResult);
	}
}

bool AGun::GunTrace(FVector& ShootDirection, FVector & EndPoint, FHitResult& HitResult)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector StartTrace = OwnerAsShooter->GetCurrentCamera()->GetComponentLocation();
	const FVector CameraDirection = OwnerAsShooter->GetCurrentCamera()->GetForwardVector();
	ShootDirection = CameraDirection;
	EndPoint = StartTrace + CameraDirection * FireRange;

	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerAsShooter);

	TArray<AActor*> OwnerTeamMates;
	UGameplayStatics::GetAllActorsOfClass(this, OwnerAsShooter->GetClass(), OwnerTeamMates);
	Params.AddIgnoredActors(OwnerTeamMates);

	return World->LineTraceSingleByChannel
	(
		HitResult,
		StartTrace,
		EndPoint,
		ECollisionChannel::ECC_Visibility,
		Params
	);
}