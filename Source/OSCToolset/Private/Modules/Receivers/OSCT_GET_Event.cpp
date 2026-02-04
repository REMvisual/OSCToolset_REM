// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Event.h"

#include "OSCManager.h"

UOSCT_GET_Event::UOSCT_GET_Event()
{
	PrimaryComponentTick.bCanEverTick = false;

	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::EVENT;
	SetDebugColor();
	DebugDuration = 2.0f;
}

void UOSCT_GET_Event::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	Get_Event.Broadcast(Address);

	OSCTDebugOSCMessage("EVENT");
}