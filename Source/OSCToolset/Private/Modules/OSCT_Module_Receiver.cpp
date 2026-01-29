// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff

#include "Modules/OSCT_Module_Receiver.h"

#include "TimerManager.h"

#include "OSCManager.h"

#include "Engine/World.h"

#include "Kismet/KismetStringLibrary.h"

UOSCT_Module_Receiver::UOSCT_Module_Receiver()
{
	PrimaryComponentTick.bCanEverTick = true;

	InterpolationSpeed = 5.0f;
	RoleType = EOSCT_Role_Type::RECEIVER;

	MessageTimeoutDuration = 0.03f; //Around 30 frames per second
	bUpdated = true;
	firstMessage = true;
}

void UOSCT_Module_Receiver::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(EnableTick);
}

void UOSCT_Module_Receiver::init_OSCT_Module()
{
	Super::init_OSCT_Module();
	if (OSCT_Master)
	{
		OSCT_Master->RegisterListener(Address, this);
	}
	////Binds the GET_Message Event to the OnOSCMessageReceived of the OSC Plugin.
	// OSCT_Server->OnOscMessageReceived.AddDynamic(this, &UOSCT_Module_Receiver::filter_OSC_Message);
	// this->OnMessageFiltered.AddDynamic(this, &UOSCT_Module_Receiver::GET_Message);
}

void UOSCT_Module_Receiver::shutdown_OSCT_Module()
{
	OSCT_Server->OnOscMessageReceived.RemoveAll(this);
	this->OnMessageFiltered.RemoveAll(this);
}

void UOSCT_Module_Receiver::ResetMessageTimer()
{
	// Ensure the receiver is valid
	if (!IsValid(this))
	{
		UE_LOG(OSCToolset, Error, TEXT("UOSCT_Module_Receiver::ResetMessageTimer() was called on an invalid object!"));
		return;
	}

	// Reset the "bUpdated" flag to indicate that messages are being received
	bUpdated = true;

	// Ensure the world is valid before using the timer manager
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(OSCToolset, Error, TEXT("ResetMessageTimer() called, but GetWorld() returned nullptr!"));
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();

	// Clear any existing timer to avoid overlapping timers
	if (TimerManager.IsTimerActive(MessageTimeoutHandle))
	{
		TimerManager.ClearTimer(MessageTimeoutHandle);
	}

	// Set a new timer. If this time expires, it means no messages were received during this period.
	GetWorld()->GetTimerManager().SetTimer(
		MessageTimeoutHandle,
		this,
		&UOSCT_Module_Receiver::OnMessageTimeout,
		MessageTimeoutDuration,
		false
	);
}

void UOSCT_Module_Receiver::OnMessageTimeout()
{
	// If this function is called, it means the timer ran out without receiving new messages
	bUpdated = false;
}

int32 UOSCT_Module_Receiver::GetMessagePackLength(const FOSCMessage& InMessage)
{
	//The component length is how many parts each component has, for example a VEC3 is 3 Components.
	//ComponentLength+1 is each element of the pack (KEY+Values)
	//The pack structure expects a string / key before the list of components, so that is why we divide by ComponentLength+1
	//InMessage Argument Length / (ComponentLength+1)
	int32 length = 0;
	UOSCManager::GetInt32(InMessage, 0, length);
	return length;
	//return InMessage.GetArgumentsChecked().Num() / (ComponentLength+1);
}

void UOSCT_Module_Receiver::filter_OSC_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort) {

	//This function filters the OSC Messages from the formatted address generated on begin play.

	FString addr_str = UOSCManager::GetOSCAddressFullPath(UOSCManager::GetOSCMessageAddress(InMessage));
	if (UKismetStringLibrary::EqualEqual_StrStr(FormattedAddress, addr_str))
	{
		//Once we checked if the Module Formatted address equals exactly to the incoming address, we can call GET_Message.
		OnMessageFiltered.Broadcast(InMessage, InAddress, InPort);
	}
}

void UOSCT_Module_Receiver::GET_Message(const FOSCMessage& InMessage, const FString& InAddress, int32 InPort)
{
	if (firstMessage == true)
	{
		//This is for the first message of the module, it calls its begin play.
		//This is used for Packs, so that we can spawn things depending on their size for example. 
		On_Init_OSCT_Module.Broadcast();
		On_Init_OSCT_Module.Clear();
		firstMessage = false;
	}
	ResetMessageTimer();
}

void UOSCT_Module_Receiver::OnOSCMessageReceived_Implementation(const FString& InAddress, const FOSCMessage& InMessage)
{
	GET_Message(InMessage, InAddress, 0);
}
