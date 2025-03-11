// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_Event_Pack.h"
#include "OSCManager.h"

UOSCT_GET_Event_Pack::UOSCT_GET_Event_Pack()
{
	PrimaryComponentTick.bCanEverTick = false;

	//Defaults for the receiver
	ModuleType = EOSCT_Module_Type::EVENT;
	SetDebugColor();

	isPack = true;
}

void UOSCT_GET_Event_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	const int32 compLength = 1;
	int32 length = GetMessagePackLength(InMessage, compLength);

	TMap < FString, bool > EventMap;

	//Compile the message values to a packed array.
	for (int i = 0; i < length; i++)
	{
		int index = (i) * 2;

		FString key = "";
		bool val = false;

		UOSCManager::GetString(InMessage, index, key);		// Pair (even) index, sequence of 0-2-4-6-...
		UOSCManager::GetBool(InMessage, index+1, val);		// Odd index is i + 1 1-3-5--9,...7

		EventMap.Add(key, val);
	}
	Pack.Map = EventMap;
	Get_Event_Pack.Broadcast(Address, Pack);

	// DEBUG //
	FString Slength = FString::FromInt(length);
	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);
}
