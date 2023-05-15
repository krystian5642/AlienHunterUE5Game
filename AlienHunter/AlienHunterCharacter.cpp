// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlienHunterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/HealthComponent.h"
#include "AI/Controllers/AIPrimitiveAlienController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Actors/DifferentActors/BloodPool.h"
#include "Components/DecalComponent.h"
#include "Components/AudioComponent.h"
#include "Actors/Characters/Cyborg.h"
#include "Actors/Items/Item.h"
#include "AlienHunter/AlienHunterGameMode.h"

//////////////////////////////////////////////////////////////////////////
// AAlienHunterCharacter

AAlienHunterCharacter::AAlienHunterCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	InitializeCharacterMovement();
	InitializeCharacterCamera();

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	InitializeHandBoxes();

	InitializeHealthBarComponent();
}

void AAlienHunterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetermineMovementMode();
}

float AAlienHunterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = 0.0f;

	if (!HealthComponent || !EventInstigator || !DamageCauser || !HealthComponent->IsAlive())
	{
		return DamageApplied;
	}

	// To avoid friendly damage
	if (const AController* const MyController = GetController())
	{
		const bool bIsAlienInstigator = EventInstigator->ActorHasTag(TEXT("AlienTeam"));
		const bool bAmIAlien = MyController->ActorHasTag(TEXT("AlienTeam"));

		if (bIsAlienInstigator && bAmIAlien)
		{
			return DamageApplied;
		}
	}

	DamageApplied = HealthComponent->ApplyDamage(DamageAmount);
	if (!HealthComponent->IsAlive())
	{
		HandleDeath();
	}
	DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageApplied && !DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		PlayDamageReactionMontage();
	}

	if (DamageApplied && TakeDamageSound && !IsValid(TakeDamageAudio))
	{
		TakeDamageAudio = UGameplayStatics::SpawnSoundAttached(TakeDamageSound, GetMesh(), TEXT("head"));
	}

	return DamageApplied;
}

void AAlienHunterCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetTargetMaxWalkSpeed();

	LeftHandBox->OnComponentBeginOverlap.AddDynamic(this,&AAlienHunterCharacter::ApplyMeleeDamage);
	RightHandBox->OnComponentBeginOverlap.AddDynamic(this, &AAlienHunterCharacter::ApplyMeleeDamage);
}

void AAlienHunterCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAlienHunterCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AAlienHunterCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AAlienHunterCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAlienHunterCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AAlienHunterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AAlienHunterCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AAlienHunterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AAlienHunterCharacter::LookUpAtRate);

	// Combat
	PlayerInputComponent->BindAction("TryToSingleAttack", IE_Pressed, this, &AAlienHunterCharacter::TryToSingleAttack);
}

void AAlienHunterCharacter::TryToSingleAttack()
{
	if (!CanHandAttack())
	{
		return;
	}

	int32 IndexMin = 0;
	int32 IndexMax = HandAttackMontage->CompositeSections.Num()-1;
	int32 Index = FMath::RandRange(IndexMin, IndexMax);

	FName SectionName= HandAttackMontage->GetSectionName(Index);
	PlayAnimMontage(HandAttackMontage, 1.0f, SectionName);
	const float AnimMontageLenght = HandAttackMontage->GetSectionLength(Index);

	GetWorldTimerManager().SetTimer(AttackDelayTimerHandle, AnimMontageLenght, false);
}

void AAlienHunterCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotator) const
{
	OutLocation = GetMesh()->GetSocketLocation("EyesSocket");
	OutRotator = GetMesh()->GetSocketRotation("EyesSocket");
}

bool AAlienHunterCharacter::CanHandAttack()
{
	const bool bIsFlying = GetCharacterMovement()->IsFlying();
	const bool bIsFalling = GetCharacterMovement()->IsFalling();
	if (IsAttacking() || bIsFlying || bIsFalling || !HandAttackMontage)
	{
		return false;
	}
	return true;
}

bool AAlienHunterCharacter::IsPlayingAnyMontageOnMesh() const
{
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!CharacterMesh)
	{
		return false;
	}

	UAnimInstance* AnimInstance = CharacterMesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return false;
	}
	return AnimInstance->IsAnyMontagePlaying();
}

UAIPerceptionComponent* AAlienHunterCharacter::GetPerceptionComponent()
{
	AAIPrimitiveAlienController* const AIPrimitiveAlienController = Cast<AAIPrimitiveAlienController>(GetController());
	if (!AIPrimitiveAlienController)
	{
		return nullptr;
	}
	return AIPrimitiveAlienController->GetPerceptionComponent();
}

void AAlienHunterCharacter::ActivateMeleeBoxes()
{
	TArray<UBoxComponent*> HandBoxes = { RightHandBox, LeftHandBox };
	for (UBoxComponent* const HandBox : HandBoxes)
	{
		HandBox->SetCollisionProfileName(TEXT("CloseCombat"));
		HandBox->SetGenerateOverlapEvents(true);
	
		HandBox->Activate();
	}
}

void AAlienHunterCharacter::DeactivateMeleeBoxes()
{
	TArray<UBoxComponent*> HandBoxes = { RightHandBox, LeftHandBox };
	for (UBoxComponent* const HandBox : HandBoxes)
	{
		HandBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HandBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		HandBox->SetGenerateOverlapEvents(false);

		HandBox->Deactivate();
	}
}

void AAlienHunterCharacter::HandleDeath()
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	AAlienHunterGameMode* const GameMode =Cast<AAlienHunterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->OnCharacterDeath(this);
	}

	DetachFromControllerPendingDestroy();
	GetMesh()->SetSimulatePhysics(true);
	SetLifeSpan(20.0f);
	GetWorldTimerManager().SetTimer(SpawnBloodPoolHandle, this, &AAlienHunterCharacter::TryToSpawnBloodPool, 0.2f, false,4.0f);
	HealthBarComponent->SetHiddenInGame(true);
}

void AAlienHunterCharacter::TryToSpawnBloodPool()
{
	UWorld* const World = GetWorld();
	if (!World || !BloodPoolClass)
	{
		GetWorldTimerManager().ClearTimer(SpawnBloodPoolHandle);
		return;
	}
	const FVector Start = GetMesh()->Bounds.Origin;
	const FVector End = Start - FVector(0,0,45);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bWasHitSomething = World->LineTraceSingleByChannel
	(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (bWasHitSomething)
	{
		GetWorldTimerManager().ClearTimer(SpawnBloodPoolHandle);
		const FVector Location = HitResult.ImpactPoint;
		const FRotator Rotation = HitResult.Normal.Rotation().Add(-90,0,0);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this,
		World->SpawnActor<ABloodPool>(BloodPoolClass,Location,Rotation,SpawnParams);
	}
	else
	{
		GetWorldTimerManager().SetTimer(SpawnBloodPoolHandle, this, &AAlienHunterCharacter::TryToSpawnBloodPool, 0.2f);
	}
}

void AAlienHunterCharacter::AdjustMaxWalkSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp
	(
		GetCharacterMovement()->MaxWalkSpeed,
		TargetMaxWalkSpeed,
		0.1
	);

	// if adjust max walk speed timer has already done its job, clear it
	const bool bShouldTimerBeStopped = FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMaxWalkSpeed,0.5);
	if (bShouldTimerBeStopped)
	{
		GetWorldTimerManager().ClearTimer(AdjustMaxWalkSpeedTimer);
	}
}

void AAlienHunterCharacter::DetermineMovementMode()
{
	if (!bAutoDetermineMovementMode)
	{
		return;
	}
	const FVector Velocity = GetVelocity();
	const float GroundSpeed = FVector(Velocity.X, Velocity.Y, 0).Size();
	if (GroundSpeed == 0 && CurrentMovementMode != EBasicMovementMode::Standing)
	{
		CurrentMovementMode = EBasicMovementMode::Standing;
	}
	else if (GroundSpeed && CurrentMovementMode == EBasicMovementMode::Standing)
	{
		CurrentMovementMode = EBasicMovementMode::Walking;
	}
}

void AAlienHunterCharacter::SetTargetMaxWalkSpeed()
{
	switch (CurrentMovementMode)
	{
		case EBasicMovementMode::Walking:
			TargetMaxWalkSpeed = MaxWalkingSpeed;
			break;

		case EBasicMovementMode::Running:
			TargetMaxWalkSpeed = MaxRunningSpeed;
			break;

		case EBasicMovementMode::Standing:
			TargetMaxWalkSpeed = 0;
			break;
	}
	const float DeltaSeconds = GetWorld()->GetDeltaSeconds();
	GetWorldTimerManager().SetTimer(AdjustMaxWalkSpeedTimer,this,&AAlienHunterCharacter::AdjustMaxWalkSpeed,DeltaSeconds,true);
}

#if WITH_EDITOR
void AAlienHunterCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	HandDamage = Strenght * HandDamageFactor;
}
#endif

void AAlienHunterCharacter::InitializeCharacterMovement()
{
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	
}

void AAlienHunterCharacter::InitializeCharacterCamera()
{
	// Creates a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Creates a third person camera
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	ThirdPersonCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void AAlienHunterCharacter::InitializeHealthBarComponent()
{
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	if (HealthBarComponent)
	{
		HealthBarComponent->SetupAttachment(RootComponent);
		HealthBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
		HealthBarComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
		static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarClass(TEXT("/Game/Blueprints/WidgetComponents/BP_HealthBar"));
		if (HealthBarClass.Succeeded())
		{
			HealthBarComponent->SetWidgetClass(HealthBarClass.Class);
		}
	}
}

void AAlienHunterCharacter::InitializeHandBoxes()
{
	const FVector BoxExtent(10, 7, 7);
	LeftHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBox"));
	LeftHandBox->SetupAttachment(GetMesh(), TEXT("hand_l"));
	LeftHandBox->SetBoxExtent(BoxExtent);

	RightHandBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandBox"));
	RightHandBox->SetupAttachment(GetMesh(), TEXT("hand_r"));
	RightHandBox->SetBoxExtent(BoxExtent);

	DeactivateMeleeBoxes();
}

void AAlienHunterCharacter::PlayDamageReactionMontage()
{
	if (!DamageReactionMontage)
	{
		return;
	}
	int32 IndexMin = 0;
	int32 IndexMax = DamageReactionMontage->CompositeSections.Num() - 1;
	int32 Index = FMath::RandRange(IndexMin, IndexMax);

	FName SectionName = DamageReactionMontage->GetSectionName(Index);
	PlayAnimMontage(DamageReactionMontage, 1.0f, SectionName);
}

void AAlienHunterCharacter::ApplyMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}

	const float DamageApplied = UGameplayStatics::ApplyDamage
	(
		OtherActor,
		HandDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}