// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Receivers/OSCT_GET_String_Pack.h"
#include "OSCManager.h"

UOSCT_GET_String_Pack::UOSCT_GET_String_Pack()
{
	PrimaryComponentTick.bCanEverTick = false;

	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::STRING;
	componentLength = 0; // The string pack doesn't have keys as it brings in an array, then we would have 2 times less strings.
	SetDebugColor();
	DebugDuration = 2.0f;

	isPack = true;
}

void UOSCT_GET_String_Pack::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{

	TArray <FString> StringArray;
	UOSCManager::GetAllStrings(InMessage, StringArray);


	Pack = StringArray;
	Get_String_Pack.Broadcast(Pack);

	// DEBUG //
	FString Slength = FString::FromInt(StringArray.Num());

	OSCTDebugOSCMessage(Slength);

	Super::GET_Message(InMessage, InAddress, InPort);

}
