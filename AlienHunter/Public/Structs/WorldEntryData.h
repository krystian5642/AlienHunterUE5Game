#pragma once

#include "Engine/DataTable.h"
#include "WorldEntryData.generated.h"

class ATeleportKey;

USTRUCT(BlueprintType)
struct FWorldEntryData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WorldEntryImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ATeleportKey> TeleportKeyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> LoadScreenClass;

};