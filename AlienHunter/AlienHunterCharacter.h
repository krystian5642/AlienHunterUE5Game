// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionListenerInterface.h"
#include "AlienHunterCharacter.generated.h"

class UBoxComponent;
class UHealthComponent;
class UWidgetComponent;
class ABloodPool;

UENUM(BlueprintType)
enum class EBasicMovementMode : uint8
{
	Standing,
	Walking,
	Running
};

UCLASS(config=Game)
class AAlienHunterCharacter : public ACharacter ,public IAIPerceptionListenerInterface
{
	GENERATED_BODY()

	// To give access to de/activate melee boxes
	friend class UMeleeAttackNotifyState;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ThirdPersonCamera;

	/** Active when there is a need of lerping current walk speed to target walk speed */
	FTimerHandle AdjustMaxWalkSpeedTimer;

	/** Two hand boxes are used to apply hand damage */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftHandBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightHandBox;

	/** Spawned when the character dies */
	UPROPERTY(EditDefaultsOnly, Category = "Visual Effects")
	TSubclassOf<ABloodPool> BloodPoolClass;

	FTimerHandle SpawnBloodPoolHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HealthBarComponent;

	/** Played when the ai character takes any damage */
	UPROPERTY(EditDefaultsOnly, Category = "Sound Effects")
	USoundBase* TakeDamageSound;

	UPROPERTY()
	UAudioComponent* TakeDamageAudio;

public:
	AAlienHunterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Called when taking damage by different game events */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Called when a characters wants to attack , if attack is impossible, it does nothing */
	virtual void TryToSingleAttack();

	/** To attach AI sight to the eyes */
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotator) const override;

	/** true if any montage is playing on the character mesh */
	virtual bool IsPlayingAnyMontageOnMesh() const;

	/** Get perception component if it is a AI character */
	virtual UAIPerceptionComponent* GetPerceptionComponent() override;

	/** true if a character can use hand attack */
	bool CanHandAttack();

	UFUNCTION(BlueprintCallable)
	/** true if the character is attacking */
	FORCEINLINE bool IsAttacking() const
	{
		return GetWorldTimerManager().IsTimerActive(AttackDelayTimerHandle);
	}

	/** Function to apply melee damage to the enemy */
	UFUNCTION()
	virtual void ApplyMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	/** Max speed when the character is walking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicMovement", meta = (ClampMin = 0.0f))
	float MaxWalkingSpeed = 150.f;

	/** Max speed when the character is running */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BasicMovement", meta = (ClampMin = 0.0f))
	float MaxRunningSpeed = 450.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BasicMovement")
	bool bAutoDetermineMovementMode = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	/** Set target max walk speed for current basic movement mode */
	void SetTargetMaxWalkSpeed();

	/** Used by melee attack notify to actvate melee boxes on hands */
	virtual void ActivateMeleeBoxes();

	/** Used by melee attack notify to deactvate melee boxes on hands */
	virtual void DeactivateMeleeBoxes();

	/** Called when the current health reaches 0 */
	virtual void HandleDeath();

	/** Trys to spawn a blood pool under the character , if there is any ground */
	void TryToSpawnBloodPool();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	/** Health component stores info about the character health */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	/** Character hand attack montages , played random section while melee attack */
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category = "Combat")
	UAnimMontage* HandAttackMontage;

	/** The reaction at taking damage, expect of radial damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	UAnimMontage* DamageReactionMontage;

	/** Active when the character is attacking */
	FTimerHandle AttackDelayTimerHandle;

	/** Character current movement mode, there are 3 basic movement modes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BasicMovement", meta = (EditCondition = "!bAutoDetermineMovementMode"))
	EBasicMovementMode CurrentMovementMode = EBasicMovementMode::Standing;

	/** We lerp to this target speed every tick */
	float TargetMaxWalkSpeed;

	/** Strenght that is used to throw a greande or apply a hand damage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = 0.0f))
	float Strenght = 1000.0f;

	/** We multiply this factor by the character strenght to get hand amount of the hand damage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = 0.0f,ClampMax = 1.0f))
	float HandDamageFactor = 0.01f;

	/** Damage applied in a one single hand attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = 0.0f))
	float HandDamage = Strenght * HandDamageFactor;

private:
	/** This function lerps to MaxWalkSpeed to get a smooth velocity change */
	void AdjustMaxWalkSpeed();

	/** This function determines movement mode base on if there is any movement */
	void DetermineMovementMode();

	/** Initialize Character movement component config settings in the constructor */
	void InitializeCharacterMovement();

	/** Initialize character camera in the constructor */
	void InitializeCharacterCamera();

	/** Initialize health bar component above the character */
	void InitializeHealthBarComponent();

	/** Initialize hand boxes */
	void InitializeHandBoxes();

	/** Plays damage reaction montage when damage type is not of radial damage */
	void PlayDamageReactionMontage();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const 
	{ 
		return CameraBoom; 
	}
	FORCEINLINE class UCameraComponent* GetThirdPersonCamera() const
	{
		return ThirdPersonCamera; 
	}

	/** Returns a current used camera **/
	virtual class UCameraComponent* GetCurrentCamera() const
	{ 
		return ThirdPersonCamera;
	}

	/** Returns current movement mode */
	FORCEINLINE EBasicMovementMode GetMovementMode() const 
	{ 
		return CurrentMovementMode; 
	}

	FORCEINLINE UHealthComponent* GetHealthComponent() const 
	{ 
		return HealthComponent;
	}

	FORCEINLINE UAnimMontage* GetHandAttackMontage() const 
	{
		return HandAttackMontage; 
	}

	FORCEINLINE EBasicMovementMode GetCurrentMovementMode() const 
	{ 
		return CurrentMovementMode; 
	}
	FORCEINLINE void SetCurrentMovementMode(EBasicMovementMode NewMovementMode)  
	{
		CurrentMovementMode = NewMovementMode;
		SetTargetMaxWalkSpeed();
	}

	FORCEINLINE float GetStrenght() const
	{
		return Strenght;
	}
	FORCEINLINE void SetStrenght(float NewThrowForce)
	{
		Strenght = FMath::Min(0, NewThrowForce);
	}

	FORCEINLINE float GetHandDamageFactor() const
	{
		return HandDamageFactor;
	}
	FORCEINLINE void SetHandDamageFactor(float NewHandDamageFactor)
	{
		HandDamageFactor = FMath::Clamp(NewHandDamageFactor, 0, 1);
	}

};

