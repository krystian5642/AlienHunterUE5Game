// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/Weapon.h"
#include "Actors/Characters/Cyborg.h"
#include "Components/EquipmentComponent.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	// Sets default values for ItemData
	ItemData.ItemType = EItemType::Weapon;
	ItemData.WeaponType = EWeaponType::Unknown;
}

float AWeapon::WeaponAttack()
{
	return PlayWeaponAttackMontageOnOwner();
}

void AWeapon::Use(AAlienHunterCharacter* Character)
{
	AShooter* const Shooter = Cast<AShooter>(Character);
	if (!IsValid(Shooter))
	{
		return;
	}

	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetSimulatePhysics(false);

	// Attaches the weapon mesh to the character mesh
	AttachToComponent
	(
		Shooter->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		ParentSocket
	);

	Shooter->SetCurrentUsedWeapon(this);
}

float AWeapon::PlayWeaponAttackMontageOnOwner()
{
	float MontageLenght = 0.0f;
	if (!OwnerAsShooter)
	{
		return MontageLenght;
	}
	UAnimMontage* const AnimMontageToPlay = OwnerAsShooter->GetWeaponAttackMontage(ItemData.WeaponType);
	if (AnimMontageToPlay)
	{
		int32 IndexMin = 0;
		int32 IndexMax = AnimMontageToPlay->CompositeSections.Num() - 1;
		int32 Index = FMath::RandRange(IndexMin, IndexMax);

		FName SectionName = AnimMontageToPlay->GetSectionName(Index);
		OwnerAsShooter->PlayAnimMontage(AnimMontageToPlay,1.0f,SectionName);
		MontageLenght = AnimMontageToPlay->GetSectionLength(Index);
	}
	return MontageLenght;
}

#if WITH_EDITOR
void AWeapon::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	MinDamage = FMath::Clamp(MinDamage, 0, MaxDamage);

}
#endif

void AWeapon::CreateItemTextInfo()
{
	Super::CreateItemTextInfo();

	ItemData.ItemTextInfo.Append("\n");

	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EWeaponType"), true);
	FString EnumString = EnumPtr ? EnumPtr->GetValueAsString(ItemData.WeaponType) : FString("Invalid");
	EnumString = EnumString.RightChop(FString("EWeaponType::").Len());

	ItemData.ItemTextInfo.Append("Weapon Type : " + EnumString + "\n");
	ItemData.ItemTextInfo.Append("Min Damage : " + FString::SanitizeFloat(MinDamage) + "\n");
	ItemData.ItemTextInfo.Append("Max Damage : " + FString::SanitizeFloat(MaxDamage));
}

void AWeapon::SpawnSurfaceReaction(const FHitResult& HitResult)
{
	const FMaterialReaction& MaterialReaction = MaterialReactions.FindRef(UGameplayStatics::GetSurfaceType(HitResult));

	if (UMaterialInterface* const DecalMaterial = MaterialReaction.DecalMaterial)
	{
		UPrimitiveComponent* const HitComponent = HitResult.GetComponent();

		const float RandFloat = FMath::RandRange(-MaterialReaction.DecalSizeYZDeviation,
			MaterialReaction.DecalSizeYZDeviation);
		const float DecalSizeY = MaterialReaction.DecalSizeBase.Y + RandFloat;
		const float DecalSizeZ = MaterialReaction.DecalSizeBase.Z + RandFloat;

		const FVector DecalSize(MaterialReaction.DecalSizeBase.X, DecalSizeY, DecalSizeZ);
		const FRotator DecalRotation = HitResult.Normal.Rotation();

		UGameplayStatics::SpawnDecalAttached(DecalMaterial, DecalSize, HitComponent,
			HitResult.BoneName, HitResult.ImpactPoint, DecalRotation,
			EAttachLocation::KeepWorldPosition, 20.0f);
	}
	if (USoundBase* const Sound = MaterialReaction.Sound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(),Sound,HitResult.ImpactPoint);
	}
	if (UParticleSystem* const Particle = MaterialReaction.Particle)
	{
		// Subtract -90,0,0 since it is a base rotation we have to keep
		const FRotator Rotation = HitResult.Normal.Rotation().Add(-90, 0, 0);
		const FVector Scale(0.5f);
		const FTransform Transform(Rotation, HitResult.ImpactPoint, Scale);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Particle,Transform);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	OwnerAsShooter = Cast<AShooter>(Owner);
}
