// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Senders/OSCT_SEND_Float.h"
#include "OSCManager.h"

UOSCT_SEND_Float::UOSCT_SEND_Float()
{
	SenderType = EOSCT_Sender_Type::FLOAT;
	ModuleType = EOSCT_ModuleType::FLOAT;
	SetDebugColor();
}

void UOSCT_SEND_Float::OSCT_Send_Float(const FString Name, const float Value, bool& bSuccess)
{
	bSuccess = true;

	if (is_address_valid(Name) == false)
	{
		bSuccess = false;
		return;
	}

	FOSCAddress addr = UOSCManager::ConvertStringToOSCAddress(FormattedAddress + "/" + Name);
	FOSCMessage msg;
	UOSCManager::AddFloat(msg, Value);
	

	if (Address != "")
	{
		OSCT_Client->SendOSCMessage(UOSCManager::SetOSCMessageAddress(msg, addr));
		OSCTDebugOSCMessage(FString::Printf(TEXT("/%s - %.2f"),*Name, Value));
	}
}
