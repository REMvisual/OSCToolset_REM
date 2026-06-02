#pragma once

#include "CoreMinimal.h"
#include "OSCT_ETypes.h"
#include "OSCToolsetEditorTypes.generated.h"

/** One inline address entry on the OSC Get/Switch Values nodes — individually typed, so the inline list is multi-value. */
USTRUCT()
struct FOSCT_NodeAddress
{
	GENERATED_BODY()

	/** OSC receiver address (short form, e.g. "KnobA1"). */
	UPROPERTY(EditAnywhere, Category="OSC")
	FString Address;

	/** Value type for this address. */
	UPROPERTY(EditAnywhere, Category="OSC")
	EOSCT_ModuleType Type = EOSCT_ModuleType::FLOAT;
};
