// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlienHunter/Public/Actors/Characters/Shooter.h"
#include "AlienHunter/Public/Structs/ItemData.h"
#include "Cyborg.generated.h"

class UEquipmentComponent;
class UCameraComponent;
class AProjectilePredictionSpline;
class AMerchant;
class UInventoryComponent;
class UQuestLogComponent;
class UWorldMapComponent;
class AQuestTracer;
struct FReward;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyValueChanged, int, Money);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemCollected , TSubclassOf<AActor>, ActorClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocationReached, const FName&, LocationName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyKilled, TSubclassOf<AActor>, EnemyClass);

/** Camera modes for characters, we can toggle between them */
UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	FirstPerson,
	Aiming,
	ThirdPersonClose,
	ThirdPersonFar
};

UCLASS()
class ALIENHUNTER_API ACyborg : public AShooter
{
	GENERATED_BODY()

	/** The box used to trigger overlap event for interacion between objects */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InteractBox;
	
	/** FirstPersonCamera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	/** We lerp to this target camera boom lenght every tick */
	float TargetCameraBoomLenght;

	/** Active when we change camera view to activate function that lerps to the target camera boom lenght */
	FTimerHandle AdjustCameraBoomLenghtTimer;

	enum class ESwitchWeaponType : uint8
	{
		Up,
		Down
	};

public:
	ACyborg();

	/** Function to switch camera mode */
	void ToggleCameraMode();

	/** Function to manually enable given camera mode */
	void EnableCameraMode(ECameraMode NewCameraMode);

	/**
	 * @return true only if player is not aiming and in a proper basic movement mode
	 */
	FORCEINLINE virtual bool CanRun()
	{
		return CurrentMovementMode != EBasicMovementMode::Standing && !bIsAiming;
	}

	/** To use items like medical or teleport key and so on */
	UFUNCTION(BlueprintCallable)
	void UseCustomItem(TSubclassOf<AItem> ItemClass);

	/**
	* @return true if the payment was successful
	*/
	bool Pay(int InPayment);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetMoney(int InMoney)
	{
		Money = FMath::Max(0, InMoney);
		OnMoneyValueChanged.Broadcast(Money);
	};

	void PresentReward(const FReward& Reward);

	virtual bool CanAim() const override;

	/** Returns a current used camera */
	virtual UCameraComponent* GetCurrentCamera() const override;

	/** Function to apply melee damage to the enemy */
	virtual void ApplyMeleeDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	/** Trys to do a single attack , hand attack by default if the characets has no weapon */
	virtual void TryToSingleAttack() override;

public:
	/**
	 * You should not modify native TargetArmLength since it has no effect when we use
	 * CloseCameraBoomLenght and FarCameraBoomLenght
	 */

	/** Close camera - lenght of camera boom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float CloseCameraBoomLenght = 200.f;

	/** Far camera - lenght of camera boom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float FarCameraBoomLenght = 400.f;

	/** Aiming camera - lenght of camera boom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float AimingCameraBoomLenght = 50.f;

	UPROPERTY(BlueprintAssignable)
	FOnMoneyValueChanged OnMoneyValueChanged;

	UPROPERTY(BlueprintAssignable)
	FOnItemCollected OnItemCollected;

	UPROPERTY(BlueprintAssignable)
	FOnLocationReached OnLocationReached;

	UPROPERTY(BlueprintAssignable)
	FOnEnemyKilled OnEnemyKilled;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	/** Used by melee attack notify to actvate melee boxes on hands or knife*/
	virtual void ActivateMeleeBoxes() override;

	/** Used by melee attack notify to deactvate melee boxes on hands or knife */
	virtual void DeactivateMeleeBoxes() override;

	/**
	 * Function begins execution after pressing the proper key. Interaction happens between objects and our character.
	 * Interaction can occur with objects that overlap with our interact capsule
	 */
	void Interact();

	/** Funtion to enable sprint */
	void EnableSprint(float AxisValue);

	/** Function to aim at target */
	void EnableAiming(float AxisValue);

	/** To start predicting a greande path if any is held in the character hands and aiming is possible */
	void StartPredictingGrenadePath();

	/**  To end predicting a greande path if any is held in the character hands , called when the character has been aiming and aiming has become inpossible */
	void EndPredictingGrenadePath();

	/** Buys an item
	* @return true if the transaction with the merchant was succesful  
	*/
	UFUNCTION(BlueprintCallable)
	bool BuyItem(const FItemData& ItemData,int Items = 1);

	/** Increments a desired weapon type in the character equipment */
	void SwitchWeaponUp();

	/** Decrements a desired weapon type in the character equipment */
	void SwitchWeaponDown();

	/** Shows an info widget " Press "E" to interact widget" above an interactable object */
	UFUNCTION()
	void ShowInteractWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Hides an info widget " Press "E" to interact widget" above an interactable object */
	UFUNCTION()
	void HideInteractWidget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	/** Camera Mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
	ECameraMode CurrentCameraMode = ECameraMode::ThirdPersonClose;

	/** This is the actual character equipment where we can store weapons only */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEquipmentComponent* EquipmentComponent;

	/** Shows " Press "E" to interact" above an interactable object */
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UWidgetComponent* InteractInfoWidget;

	/** This is the actual character inventory when we can store all items */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UQuestLogComponent* QuestLogComponent;
	
	/** Grenade prediction path class */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AProjectilePredictionSpline> GrenadePredictionPathClass;

	/** Grenade prediction path , spawned at the begin play and hidden set to true */
	UPROPERTY()
	AProjectilePredictionSpline* GrenadePredictionPath;

	/** The current merchant this character is trading with */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	AMerchant* Merchant;

	/** The actual ammount of money that the cyborg can use in trading with a merchant */
	UPROPERTY(EditAnywhere,BlueprintSetter = SetMoney, BlueprintReadWrite, Category = "Inventory", meta = (ClampMin = 0.0f))
	int Money = 1000;

private:
	/** Initialize interact info widget */
	void InitializeInteractInfoWidget();

	/** This function lerps to the needed camera boom lenght depending on CurrentCameraMode */
	void AdjustCameraBoomLenght();

	/** Called to start aiming */
	void StartAiming();

	/** Called to end aiming */
	void EndAiming();

	/**
	* Internal switch weapon up or down
	*/
	void InternalSwitchWeapon(ESwitchWeaponType SwitchWeaponType);

	void ToggleFleshlight();

public:
	/** Returns a current used camera mode */
	FORCEINLINE ECameraMode GetCameraMode() const  
	{
		return CurrentCameraMode;
	}

	FORCEINLINE UEquipmentComponent* GetEquipmentComponent() const 
	{
		return EquipmentComponent;
	}

	FORCEINLINE UInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent;
	}

	FORCEINLINE UQuestLogComponent* GetQuestLogComponent() const
	{
		return QuestLogComponent;
	}



};
