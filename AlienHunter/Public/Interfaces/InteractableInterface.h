// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class ACyborg;

class ALIENHUNTER_API IInteractableInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 
	 * Function to interact between actors. Returns true if interaction was successful
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interact")
	bool Interact(ACyborg* Character);

};
