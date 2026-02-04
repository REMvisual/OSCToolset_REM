// Fill out your copyright notice in the Description page of Project Settings.


#include "Modules/Receivers/OSCT_GET_String.h"

#include "OSCManager.h"

UOSCT_GET_String::UOSCT_GET_String() 
{
	//Defaults for the receiver
	ModuleType = EOSCT_ModuleType::STRING;
	SetDebugColor();
}

void UOSCT_GET_String::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	Super::GET_Message(InMessage, InAddress, InPort);

	UOSCManager::GetString(InMessage, 0, Value);
	Get_String.Broadcast(Address, Value);
	OSCTDebugOSCMessage(Value);
}

