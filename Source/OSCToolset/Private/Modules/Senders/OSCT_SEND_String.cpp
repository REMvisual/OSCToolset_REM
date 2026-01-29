// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Senders/OSCT_SEND_String.h"
#include "OSCManager.h"

UOSCT_SEND_String::UOSCT_SEND_String()
{
	SenderType = EOSCT_Sender_Type::STRING;
	ModuleType = EOSCT_Module_Type::STRING;
	SetDebugColor();
}

void UOSCT_SEND_String::OSCT_Send_String(const FString Name, const FString Value, bool& bSuccess)
{
	bSuccess = true;

    if (is_address_valid(Name) == false	)
    {
		bSuccess = false;
        return;
    }

	FOSCAddress addr = UOSCManager::ConvertStringToOSCAddress(FormattedAddress + "/" + Name);
	FOSCMessage msg;
	UOSCManager::AddString(msg, Value);
	
	if (OSCT_Client)
	{
		OSCT_Client->SendOSCMessage(UOSCManager::SetOSCMessageAddress(msg, addr));
		OSCTDebugOSCMessage(FString::Printf(TEXT("/%s - %s"), *Name, *Value));
	}
}