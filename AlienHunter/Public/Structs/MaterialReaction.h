#pragma once

#include "MaterialReaction.generated.h"

/*
* Struct FMaterialReaction is used to spawn proper visual and sound effects
* when we use any weapon against a physical surface type
**/

USTRUCT(BlueprintType)
struct FMaterialReaction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* Particle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DecalSizeBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecalSizeYZDeviation;

};