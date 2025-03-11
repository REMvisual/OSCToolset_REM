// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/OSCT_Module_Sender.h"

#include "OSCManager.h"

#include "Engine/Engine.h"

UOSCT_Module_Sender::UOSCT_Module_Sender()
{
	RoleType = EOSCT_Role_Type::SENDER;

}

void UOSCT_Module_Sender::SEND_Message()
{

}

void UOSCT_Module_Sender::BeginPlay()
{
	Super::BeginPlay();
}

void UOSCT_Module_Sender::init_OSCT_Module()
{
	Super::init_OSCT_Module();
	//UE_LOG(OSCToolset, Log, TEXT("The sender has initialized"));
	////Binds the GET_Message Event to the OnOSCMessageReceived of the OSC Plugin.
	//Client-OnOscMessageReceived.AddDynamic(this, &UOSCT_Module_Receiver::filter_OSC_Message);//
	//this->OnMessageFiltered.AddDynamic(this, &UOSCT_Module_Receiver::GET_Message
}

bool UOSCT_Module_Sender::is_name_valid(FString Name)
{

    // Initialize success as true
    bool bSuccess = true;

    // Check if the name contains whitespaces
    if (Name.Contains(" "))
    {
        // Set success to false to indicate failure
        bSuccess = false;

        // Log the error message (this will appear in the Output Log)
        UE_LOG(LogTemp, Error, TEXT("OSCT_Send_String: Name '%s' contains whitespaces!"), *Name);

        // Optionally, print a warning to the screen for debugging purposes
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error: Name contains whitespaces!"));
        }

        // Early return on invalid input
        return bSuccess;
    }

	return bSuccess;
}
