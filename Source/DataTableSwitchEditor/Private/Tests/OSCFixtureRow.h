#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OSCFixtureRow.generated.h"

USTRUCT()
struct FOSCFixtureRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY()
	FString Address;

	UPROPERTY()
	FString ModuleType;
};
