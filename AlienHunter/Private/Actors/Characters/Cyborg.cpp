// Fill out your copyright notice in the Description page of Project Settings.


#include "AlienHunter/Public/Actors/Characters/Cyborg.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/InteractableInterface.h"
#include "Components/EquipmentComponent.h"
#include "Actors/Weapons/Weapon.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/DifferentActors/ProjectilePredictionSpline.h"
#include "Actors/Weapons/Grenade.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Actors/Weapons/Knife.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/DifferentActors/Merchant.h"
#include "Components/InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/QuestLogComponent.h"
#include "AlienHunter/Public/Structs/QuestData.h"
#include "Actors/Weapons/Gun.h"

ACyborg::ACyborg()
{
	//Creates a first person camera
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->AttachToComponent
	(
		GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		TEXT("head")
	);
	FirstPersonCamera->bUsePawnControlRotation = true;
	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractCapsule"));
	InteractBox->SetupAttachment(RootComponent);

	// Creates a character invenory
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Creates a character equipment
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));

	InitializeInteractInfoWidget();

	QuestLogComponent = CreateDefaultSubobject<UQuestLogComponent>(TEXT("QuestLogComponent"));
}

void ACyborg::ToggleCameraMode()
{
	// Cannot switch camera mode if a shooter is aiming
	if (CurrentCameraMode != ECameraMode::Aiming)
	{
		switch (CurrentCameraMode)
		{
		case ECameraMode::ThirdPersonClose:
			EnableCameraMode(ECameraMode::ThirdPersonFar);
			break;

		case ECameraMode::ThirdPersonFar:
			EnableCameraMode(ECameraMode::FirstPerson);
			break;

		case ECameraMode::FirstPerson:
			EnableCameraMode(ECameraMode::ThirdPersonClose);
			break;

		default:
			break;
		}
	}
}

void ACyborg::EnableCameraMode(ECameraMode NewCameraMode)
{
	// Cannot activate third person aiming when first person camera is enabled
	if (NewCameraMode == ECameraMode::Aiming && NewCameraMode != ECameraMode::FirstPerson)
	{
		TargetCameraBoomLenght = AimingCameraBoomLenght;
		CurrentCameraMode = NewCameraMode;
	}
	else if (NewCameraMode != ECameraMode::FirstPerson)
	{
		// Deactivates a first person camera and activate third person camera
		FirstPersonCamera->SetActive(false);
		GetThirdPersonCamera()->SetActive(true);
		bUseControllerRotationYaw = false;
		if (CurrentCameraMode == ECameraMode::FirstPerson && NewCameraMode == ECameraMode::ThirdPersonClose)
		{
			GetCameraBoom()->TargetArmLength = CloseCameraBoomLenght;
		}
		CurrentCameraMode = (NewCameraMode == ECameraMode::ThirdPersonClose) ? ECameraMode::ThirdPersonClose : ECameraMode::ThirdPersonFar;
		TargetCameraBoomLenght = (CurrentCameraMode == ECameraMode::ThirdPersonClose) ? CloseCameraBoomLenght : FarCameraBoomLenght;
	}
	else
	{
		// Activates a first person camera and deactivate a third person camera
		FirstPersonCamera->SetActive(true);
		GetThirdPersonCamera()->SetActive(false);
		bUseControllerRotationYaw = true;
		CurrentCameraMode = ECameraMode::FirstPerson;
	}

	// Start Adjusting Camera Boom Lenght
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	GetWorldTimerManager().SetTimer(AdjustCameraBoomLenghtTimer, this, &ACyborg::AdjustCameraBoomLenght, DeltaSeconds, true);
	
}

void ACyborg::TryToSingleAttack()
{
	if (IsAttacking())
	{
		return;
	}
	Super::TryToSingleAttack();

	const bool bIsPlayingAnyMontage = IsPlayingAnyMontageOnMesh();
	AGrenade* const CurrentUsedGrenade = Cast<AGrenade>(CurrentUsedWeapon);
	const bool bIsThrowingGrenade = bIsPlayingAnyMontage && IsValid(CurrentUsedGrenade);
	if (bIsThrowingGrenade)
	{
		if (GrenadePredictionPath)
		{
			GrenadePredictionPath->EndPredictingPath();
		}
		CurrentUsedWeapon = nullptr;
		InventoryComponent->RemoveItem(CurrentUsedGrenade->ItemData.ItemClass);
	}
}

void ACyborg::UseCustomItem(TSubclassOf<AItem> ItemClass)
{
	if (!ItemClass)
	{
		return;
	}
	AItem* const Item = ItemClass.GetDefaultObject();
	if (Item)
	{
		Item->Use(this);
	}
	InventoryComponent->RemoveItem(ItemClass);
}

bool ACyborg::Pay(int InPayment)
{
	if (Money == 0 || InPayment <= 0 || Money < InPayment)
	{
		return false;
	}
	InPayment = FMath::Max(0, InPayment);
	Money -= InPayment;
	SetMoney(Money);
	return true;
}

void ACyborg::PresentReward(const FReward& Reward)
{
	if (Reward.CurrencyReward > 0)
	{
		SetMoney(Money + Reward.CurrencyReward);
	}

	for (auto [ItemClass, ItemCount] : Reward.ItemRewardClasses)
	{
		if (!ItemClass)
		{
			continue;
		}
		ItemCount = FMath::Max(ItemCount, 1);
		InventoryComponent->AddItem(ItemClass.GetDefaultObject()->ItemData, ItemCount);
	}
}

bool ACyborg::CanAim() const
{
	 /*
	 * if a greande anim montage is playing, the player cannot aim, it's especially important 
	 * when we are throwing the grenade. This excludes showing the next grenade prediction path during anim montage
	 */
	const UAnimMontage* const GrenadeMontage = GetWeaponAttackMontage(EWeaponType::Grenade);
	const bool bIsNotPlayingGrenadeMontage = GetMesh()->GetAnimInstance()->Montage_GetIsStopped(GrenadeMontage);
	return EquipmentComponent->IsWeaponEquiped(CurrentUsedWeapon) && Super::CanAim() && bIsNotPlayingGrenadeMontage;
}


UCameraComponent* ACyborg::GetCurrentCamera() const
{
	return (FirstPersonCamera->IsActive()) ? FirstPersonCamera : GetThirdPersonCamera();
}

void ACyborg::ApplyMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKnife* const CurrentUsedKnife = Cast<AKnife>(CurrentUsedWeapon);
	if (CurrentUsedKnife && OtherActor!=this)
	{
		const float MinDamage = CurrentUsedKnife->GetMinDamage();
		const float MaxDamage = CurrentUsedKnife->GetMaxDamage();
		const float CurrentDamage = FMath::RandRange(MinDamage, MaxDamage);
		UGameplayStatics::ApplyDamage
		(
			OtherActor,
			CurrentDamage,
			GetController(),
			this,
			UDamageType::StaticClass()
		);
	}
	else
	{
		Super::ApplyMeleeDamage(OverlappedComponent,OtherActor,OtherComp,OtherBodyIndex,bFromSweep,SweepResult);
	}
}

void ACyborg::BeginPlay()
{
	Super::BeginPlay();
	EnableCameraMode(CurrentCameraMode);

	if (GrenadePredictionPathClass)
	{
		GrenadePredictionPath = GetWorld()->SpawnActor<AProjectilePredictionSpline>(GrenadePredictionPathClass);
		GrenadePredictionPath->SetActorHiddenInGame(true);
	}

	if (InteractBox)
	{
		InteractBox->OnComponentBeginOverlap.AddDynamic(this,&ACyborg::ShowInteractWidget);
		InteractBox->OnComponentEndOverlap.AddDynamic(this, &ACyborg::HideInteractWidget);
	}
}

void ACyborg::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Actions
	PlayerInputComponent->BindAction("ToggleCameraMode", IE_Pressed, this, &ACyborg::ToggleCameraMode);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ACyborg::Interact);
	PlayerInputComponent->BindAction(TEXT("SwitchWeaponUp"), IE_Pressed, this, &ACyborg::SwitchWeaponUp);
	PlayerInputComponent->BindAction(TEXT("SwitchWeaponDown"), IE_Pressed, this, &ACyborg::SwitchWeaponDown);

	// Axis
	PlayerInputComponent->BindAxis("Running", this, &ACyborg::EnableSprint);
	PlayerInputComponent->BindAxis("Aiming", this, &ACyborg::EnableAiming);
}

void ACyborg::Interact()
{
	const USceneComponent* const InfoWidgetAttachParent = InteractInfoWidget->GetAttachParent();
	if (!InfoWidgetAttachParent)
	{
		return;
	}
	AActor* const Actor = InfoWidgetAttachParent->GetOwner();
	if (!Actor || Actor == this)
	{
		return;
	}
	IInteractableInterface::Execute_Interact(Actor, this);
}

void ACyborg::EnableSprint(float AxisValue)
{
	EBasicMovementMode PreviousMovementMode = CurrentMovementMode;
	CurrentMovementMode = AxisValue && CanRun() ? EBasicMovementMode::Running : EBasicMovementMode::Walking;
	const bool bWasMovementModeChanged = PreviousMovementMode != CurrentMovementMode;
	if (bWasMovementModeChanged)
	{
		SetTargetMaxWalkSpeed();
	}
}

void ACyborg::EnableAiming(float AxisValue)
{
	const bool bCanAim = CanAim();
	const bool bWasAiming = bIsAiming;
	bIsAiming = (AxisValue && bCanAim) ? true : false;
	if (bWasAiming && !bIsAiming)
	{
		EndAiming();
	}
	if(bIsAiming)
	{
		StartAiming();
	}
}

void ACyborg::StartPredictingGrenadePath()
{
	AGrenade* const CurrentUsedGrenade = Cast<AGrenade>(CurrentUsedWeapon);
	if (GrenadePredictionPath && CurrentUsedGrenade)
	{
		const float BlendToAimPosTime = 0.22f;
		GrenadePredictionPath->StartPredictingPath(CurrentUsedGrenade, BlendToAimPosTime);
		GrenadePredictionPath->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentUsedGrenade->GetParentSocket());
	}
}

void ACyborg::EndPredictingGrenadePath()
{
	if (GrenadePredictionPath)
	{
		GrenadePredictionPath->EndPredictingPath();
	}
}

void ACyborg::InitializeInteractInfoWidget()
{
	InteractInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractInfoWidget"));
	InteractInfoWidget->SetWorldLocation(FVector(0, 0, 30));
	InteractInfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractInfoWidget->SetHiddenInGame(true);
}

void ACyborg::AdjustCameraBoomLenght()
{
	if (CurrentCameraMode != ECameraMode::FirstPerson)
	{
		GetCameraBoom()->TargetArmLength = FMath::Lerp
		(
			GetCameraBoom()->TargetArmLength,
			TargetCameraBoomLenght,
			0.025
		);
	}

	// if the adjust camera boom lenght timer has already done its job, clear it
	const bool bShouldTimerBeStopped = FMath::IsNearlyEqual(GetCameraBoom()->TargetArmLength, TargetCameraBoomLenght, 0.5);
	if (bShouldTimerBeStopped)
	{
		GetWorldTimerManager().ClearTimer(AdjustCameraBoomLenghtTimer);
	}
}

void ACyborg::StartAiming()
{
	if (CurrentCameraMode != ECameraMode::FirstPerson)
	{
		bUseControllerRotationYaw = true;
	}

	GetCharacterMovement()->bOrientRotationToMovement = false;

	const bool bCanSetAimingCamera = bIsAiming && CurrentCameraMode != ECameraMode::FirstPerson
		&& CurrentCameraMode != ECameraMode::Aiming && CurrentUsedWeapon->ItemData.WeaponType!=EWeaponType::Grenade;
	if (bCanSetAimingCamera)
	{
		// Aiming has started
		EnableCameraMode(ECameraMode::Aiming);
	}
	if (CurrentUsedWeapon->ItemData.WeaponType == EWeaponType::Grenade)
	{
		StartPredictingGrenadePath();
	}
}

void ACyborg::EndAiming()
{
	if (CurrentCameraMode != ECameraMode::FirstPerson)
	{
		bUseControllerRotationYaw = false;
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (CurrentCameraMode != ECameraMode::FirstPerson)
	{
		// Aiming has ended
		EnableCameraMode(ECameraMode::ThirdPersonClose);
	}

	if (CurrentUsedWeapon && CurrentUsedWeapon->ItemData.WeaponType == EWeaponType::Grenade)
	{
		EndPredictingGrenadePath();
	}

}

void ACyborg::InternalSwitchWeapon(ESwitchWeaponType SwitchWeaponType)
{
	EWeaponType& DesiredWeaponType = EquipmentComponent->DesiredWeaponType;
	if (CurrentUsedWeapon)
	{
		DesiredWeaponType = CurrentUsedWeapon->ItemData.WeaponType;

		if (CurrentUsedWeapon->ItemData.WeaponType == EWeaponType::Grenade)
		{
			EndPredictingGrenadePath();
		}
	}
	int WeaponIndex = static_cast<int>(DesiredWeaponType);

	switch (SwitchWeaponType)
	{
		case ESwitchWeaponType::Up:
			++WeaponIndex;
			WeaponIndex = WeaponIndex % 5 ? WeaponIndex % 5 : WeaponIndex % 5 + 1;
			break;

		case ESwitchWeaponType::Down:
			--WeaponIndex;
			WeaponIndex = WeaponIndex ? WeaponIndex : 4;
			break;

		default: 
			break;
	}

	DesiredWeaponType = static_cast<EWeaponType>(WeaponIndex);
	EquipmentComponent->SetCurrentUsedWeaponType(DesiredWeaponType);
}

bool ACyborg::BuyItem(const FItemData& ItemData,int Items)
{
	if (!IsValid(Merchant) || !Items)
	{
		return false;
	}
	const bool bWasTransactionSuccessful = Merchant->SellItem(ItemData,Items);
	if (bWasTransactionSuccessful)
	{
		InventoryComponent->AddItem(ItemData, Items);
	}
	
	return bWasTransactionSuccessful;
}

void ACyborg::SwitchWeaponUp()
{
	if (!EquipmentComponent || EquipmentComponent->IsEmpty() || IsAttacking())
	{
		return;
	}
	InternalSwitchWeapon(ESwitchWeaponType::Up);
}

void ACyborg::SwitchWeaponDown()
{
	if (!EquipmentComponent || EquipmentComponent->IsEmpty() || IsAttacking())
	{
		return;
	}
	InternalSwitchWeapon(ESwitchWeaponType::Down);
}

void ACyborg::ShowInteractWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	const bool bDoesImplementInterface = OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass());
	if (!bDoesImplementInterface)
	{
		return;
	}
	InteractInfoWidget->AttachToComponent(OtherComp,FAttachmentTransformRules::KeepRelativeTransform);
	InteractInfoWidget->SetHiddenInGame(false);
}

void ACyborg::HideInteractWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	const bool bDoesImplementInterface = OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass());
	if (!bDoesImplementInterface)
	{
		return;
	}
	InteractInfoWidget->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	InteractInfoWidget->SetWorldLocation(FVector(0, 0, 30));
	InteractInfoWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	InteractInfoWidget->SetHiddenInGame(true);
}

void ACyborg::ActivateMeleeBoxes()
{
	AKnife* const CurrentUsedKnife = Cast<AKnife>(CurrentUsedWeapon);
	if (CurrentUsedKnife)
	{
		// Activate stab box
		CurrentUsedKnife->ActivateStabBox();
	}
	else
	{
		// Activate hand boxes
		Super::ActivateMeleeBoxes();
	}
}

void ACyborg::DeactivateMeleeBoxes()
{
	AKnife* const CurrentUsedKnife = Cast<AKnife>(CurrentUsedWeapon);
	if (CurrentUsedKnife)
	{
		// Deactivate stab box
		CurrentUsedKnife->DeactivateStabBox();
	}
	else
	{
		// Deactivate hand boxes
		Super::DeactivateMeleeBoxes();
	}
}
