// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/Senders/OSCT_SEND_Event.h"
#include "OSCManager.h"
#include "Engine/World.h"

UOSCT_SEND_Event::UOSCT_SEND_Event()
{
	SenderType = EOSCT_Sender_Type::EVENT;
	ModuleType = EOSCT_ModuleType::EVENT;
	SetDebugColor();
}

void UOSCT_SEND_Event::OSCT_Send_Event(const FString Name, bool& bSuccess)
{
	bSuccess = true;

	if (is_address_valid(Name) == false)
	{
		bSuccess = false;
		return;
	}

	float gameTime = GetWorld()->GetRealTimeSeconds();

	FOSCAddress addr = UOSCManager::ConvertStringToOSCAddress(FormattedAddress + "/" + Name);
	FOSCMessage msg;
	UOSCManager::AddFloat(msg, gameTime);

	if (OSCT_Client) 
	{
		OSCT_Client->SendOSCMessage(UOSCManager::SetOSCMessageAddress(msg, addr));
		OSCTDebugOSCMessage(Name);
	}
}
