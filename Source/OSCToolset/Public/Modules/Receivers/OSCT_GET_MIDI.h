// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/OSCT_Module_Receiver.h"
#include "OSCT_GET_MIDI.generated.h"


USTRUCT(BlueprintType)
struct FOSCTMidi
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	int32 Pitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	int32 Velocity;

};

UCLASS()
class OSCTOOLSET_API UOSCT_GET_MIDI : public UOSCT_Module_Receiver
{
	GENERATED_BODY()
	
};
