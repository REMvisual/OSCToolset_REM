// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Receivers/OSCT_GET_MIDI.h"

#include "OSCManager.h"


UOSCT_GET_MIDI::UOSCT_GET_MIDI()
{
	PrimaryComponentTick.bCanEverTick = false;
	ModuleType = EOSCT_ModuleType::NOTE;
	SetDebugColor();
	DebugDuration = 2.0f;
}

void UOSCT_GET_MIDI::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);
	
	int32 tempPitch = 0, tempVelocity = 0, tempVoices = 0;

	UOSCManager::GetInt32(InMessage, 0, tempPitch);
	UOSCManager::GetInt32(InMessage, 1, tempVelocity);
	UOSCManager::GetInt32(InMessage, 2, tempVoices);

	Value.Pitch = tempPitch;
	Value.Velocity = tempVelocity;
	Value.Voices = tempVoices;

	Get_MIDI.Broadcast(Address, Value);

	FString MIDIinfo = "Pitch: " + FString::FromInt(tempPitch) + " Velocity: " + FString::FromInt(tempVelocity) + " Voices: " + FString::FromInt(tempVoices);
	OSCTDebugOSCMessage(MIDIinfo);
}
