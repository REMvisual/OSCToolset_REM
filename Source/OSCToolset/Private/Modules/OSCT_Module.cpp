// Copyright (c) 2024, Studio Eusebi Jucgla. All rights reserved 
// MD5: 2245b93ed1bae2e4aff277e6f3ecb8ff
#include "Modules/OSCT_Module.h"

#include "Engine/Engine.h" // Include GEngine
#include "Engine/GameInstance.h"

#include "OSCManager.h"
#include "OSCAddress.h"

#include "OSCT_Master.h"
	
#include <Kismet/GameplayStatics.h>
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(OSCToolset);

// Sets default values for this component's properties
UOSCT_Module::UOSCT_Module()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;


	// MODULE //
	isPack = false;
	//SourceType = EOSCT_Source_Type::TD;
	
	// DEBUG // 
	Debug = false;
	DebugColor = FColor::Cyan;
	DebugDuration = 0.2f;
	PrintOnLog = false;
	PrintOnScreen = true;
	DebugAddress = false;

	// DEBUG MODULE DATA //
	OSCTMName = UKismetSystemLibrary::GetObjectName(this);
	OSCTMClass = GetClass()->GetName();
	OSCTModuleDebugString = "Name: " + OSCTMName + " | Class: " + OSCTMClass;

}

void UOSCT_Module::OSCTModuleError(const FString Message)
{
	FString error_msg = OSCTModuleDebugString + " " + Message;
	// Optionally, print a warning to the screen for debugging purposes
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, error_msg);
	}
	UE_LOG(OSCToolset, Error, TEXT("%s"), *error_msg);
}

void UOSCT_Module::SetDebugColor()
{
	switch (ModuleType)
	{
	case EOSCT_Module_Type::EVENT:
		DebugColor = FColor(255, 0, 100);
		break;
	case EOSCT_Module_Type::FLOAT:
		DebugColor = FColor(80, 255, 0);
		break;
	case EOSCT_Module_Type::VEC2:
		DebugColor = FColor(0, 200, 255);
		break;
	case EOSCT_Module_Type::VEC3:
		DebugColor = FColor(230, 200, 0);
		break;
	case EOSCT_Module_Type::COLOR:
		DebugColor = FColor(0, 50, 255);
		break;
	case EOSCT_Module_Type::TRANSFORM:
		DebugColor = FColor(255, 127, 0);
		break;
	case EOSCT_Module_Type::STRING:
		DebugColor = FColor(230, 76, 255);
		break;
	case EOSCT_Module_Type::ROTATION:
		DebugColor = FColor(127, 140, 255);
		break;
	}
}

bool UOSCT_Module::is_address_valid(const FString InAddress)
{
	// Initialize success as true
	bool bSuccess = true;

	if (InAddress.IsEmpty())
	{
		// Log the error message (this will appear in the Output Log)
		FString error_msg = "The address should not be empty.";
		OSCTModuleError(error_msg);
		return false;
	}
	// Check if the name contains whitespaces
	if (InAddress.Contains(" "))
	{
		// Set success to false to indicate failure
		bSuccess = false;
		FString error_msg = "Addresses can't containt whitespaces (' ')" + InAddress;
		OSCTModuleError(error_msg);

		// Early return on invalid input
		return bSuccess;
	}
	return bSuccess;
}

// Called when the game starts
void UOSCT_Module::BeginPlay()
{
	Super::BeginPlay();

	
	//Get owner of the module at begin play
	OSCTMOwner = GetOwner()->GetName();
	OSCTModuleDebugString += " | Owner: " + OSCTMOwner + " ";

	//From the parameters set on our module, format the Address. 
	FormatOSCTAddress();

	//Set Server / Client with the OSC_Master Game Instance Subsystem

	// Accessing the OSCT_Master
	OSCT_Master = GetWorld()->GetGameInstance()->GetSubsystem<UOSCT_Master>();

	if (OSCT_Master)
	{
		OSCT_Master->OnInitOSCT.AddDynamic(this, &UOSCT_Module::init_OSCT_Module); //Used to init the module if we call the Re-Initialize OSCT.
		OSCT_Master->OnShutdownOSCT.AddDynamic(this, &UOSCT_Module::shutdown_OSCT_Module); //Used to shutdown the module if we call the Re-Initialize OSCT.
		init_OSCT_Module(); //This is only once at Begin Play. 
	}
	else {
		UE_LOG(OSCToolset, Error, TEXT("Failed to access OSCT_MasterSubsystem"));
	}
}

void UOSCT_Module::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(OSCToolset, Log, TEXT("OSCTModule EndPlay >> NAME:%s"), *OSCTMName);
}

void UOSCT_Module::init_OSCT_Module()
{
	is_address_valid(Address); //Check if the address is valid
	Settings = OSCT_Master->Settings; //Set settings for the Module. 

	//This function is called once the Game Mode has created the Server and the Client
	//Previously I used a timer to check the validity of the Game Mode, now this seems to be much faster.
	Server = OSCT_Master->OSCT_Server;
	Client = OSCT_Master->OSCT_Client;

	FString BoolAsString = FString::Printf(TEXT("%s"), PrimaryComponentTick.bCanEverTick ? TEXT("True") : TEXT("False"));
	UE_LOG(OSCToolset, Log, TEXT(">>OSCTModule initialized<< NAME:%s // CLASS:%s // Owner:%s // CanTick:%s // Address:%s // FormattedAddress:%s"), *OSCTMName, *OSCTMClass, *OSCTMOwner, *BoolAsString, *Address, *FormattedAddress);

	//Send the string when an OSC Module is initialized, this is parsed in the Receriver to debug the connected modules.
	SendConnectedOSCTModule();

	if (DebugAddress)
	{
		OSCTDebugOSCMessage(""); // To debug the address on Begin Play of the OSCT Module
	}
	if (RoleType == EOSCT_Role_Type::SENDER)
	{
		//Call the Init Module Delegate for senders.
		On_Init_OSCT_Module.Broadcast();
		//For RECEIVERS this is called after the first message has been received.
		//This way, I can fetch the map length and values and so on. 
	}
}

void UOSCT_Module::shutdown_OSCT_Module()
{
	//Add some logic to reset the variables?
	//This is needed for OSCT_Module_Receiver, to unbind the Delegates of Sending and Filtering messages. 
}

void UOSCT_Module::OSCTDebugOSCMessage(const FString MessageValues)
{ 
	if (Debug)
	{
		FString debug_msg = OSCTModuleDebugString + FormattedAddress + ":" + MessageValues;

		if (PrintOnLog)
		{
			UE_LOG(OSCToolset, Log, TEXT("%s"), *debug_msg);
		}

		if (PrintOnScreen)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, DebugDuration, DebugColor, debug_msg);
			}
		}
	}
}

void UOSCT_Module::FormatOSCTAddress()
{
	//This function takes certain properties of the module to create the final address. Like the Source, the Address, the OP Name, the Type, etc. 
	//FString source = UKismetStringLibrary::ToUpper(UKismetStringLibrary::GetSubstring(UEnum::GetDisplayValueAsText(SourceType).ToString()));
	FString roleType = UKismetStringLibrary::ToUpper(UKismetStringLibrary::GetSubstring(UEnum::GetDisplayValueAsText(RoleType).ToString()));
	FString moduletype = GetEnumValueAsString(ModuleType);

	FormattedAddress = "/OSCT/" + roleType + "/" + moduletype;
	if (isPack)
	{
		FormattedAddress += "/p";
	}

	if (Address != "")
	{
		FormattedAddress += "/" + Address;
	}
}

void UOSCT_Module::SendConnectedOSCTModule()
{
	FOSCMessage msg;
	FString m_send_addr = "/OSCT/module_connected";

	//Name
	FString m_name = "Name%";
	m_name += UKismetSystemLibrary::GetObjectName(this);
	UOSCManager::AddString(msg, m_name);

	//Class
	FString m_class = "Class%" + GetClass()->GetName();
	UOSCManager::AddString(msg, m_class);

	//Owner
	FString m_owner = "Actor Owner%" + GetOwner()->GetName();
	UOSCManager::AddString(msg, m_owner);

	//Address
	FString m_addr = "Address%" + Address;
	UOSCManager::AddString(msg, m_addr);

	//Formatted Addr
	FString m_faddr = "Formatted Address%" + FormattedAddress;
	UOSCManager::AddString(msg, m_faddr);

	//Role type
	FString m_bt = "Role Type%" + UEnum::GetDisplayValueAsText(RoleType).ToString().ToUpper();
	UOSCManager::AddString(msg, m_bt);

	//Module type
	FString m_mt = "ModuleType%" + GetEnumValueAsString(ModuleType).ToUpper();
	UOSCManager::AddString(msg, m_mt);

	//Is Pack
	FString m_pack = "Pack%" + UKismetStringLibrary::Conv_BoolToString(isPack);
	UOSCManager::AddString(msg, m_pack);

	FOSCAddress m_osc_addr = UOSCManager::ConvertStringToOSCAddress(m_send_addr);
	Client->SendOSCMessage(UOSCManager::SetOSCMessageAddress(msg, m_osc_addr));
}

// Called every frame
void UOSCT_Module::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


template<typename TEnum>
FString UOSCT_Module::GetEnumValueAsString(TEnum InEnum)
{
	FString e_str = UEnum::GetValueAsString(InEnum);
	FString l_str;
	FString r_str;
	UKismetStringLibrary::Split(e_str, "::", l_str, r_str, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	return r_str;
}
