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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSCToolset")
	int32 Voices;
};

//Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetMIDI, FString, Address, const FOSCTMidi, MIDI);

UCLASS(meta = (BlueprintSpawnableComponent))
class OSCTOOLSET_API UOSCT_GET_MIDI : public UOSCT_Module_Receiver
{
	GENERATED_BODY()

public:
	UOSCT_GET_MIDI();

	FOSCTMidi Value;

	UPROPERTY(BlueprintAssignable)
	FOnGetMIDI Get_MIDI;

	virtual void GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) override;
};
