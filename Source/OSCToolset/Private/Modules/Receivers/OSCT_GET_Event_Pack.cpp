// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Event_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Event_Pack::UOSCT_GET_Event_Pack()
{
	PrimaryComponentTick.bCanEverTick = false;

	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::EVENT;
	componentLength = 1;
	SetDebugColor();

	isPack = true;
}

void UOSCT_GET_Event_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	int32 length = GetMessagePackLength(InMessage);

	TMap < FString, bool > EventMap;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i + 1) * 2;

		FString key = "";
		bool val = false;

		UOSCManager::GetString(InMessage, index-1, key);		// Pair (even) index, sequence of 0-2-4-6-...
		UOSCManager::GetBool(InMessage, index, val);		// Odd index is i + 1 1-3-5--9,...7

		EventMap.Add(key, val);
	}
	Pack.Map = EventMap;
	Get_Event_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);
}
