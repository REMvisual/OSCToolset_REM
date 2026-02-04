// Fill out your copyright notice in the Description page of Project Settings.

#include "Functions/OSCT_Functions.h"

#include "OSCManager.h"
#include "OSCClient.h"
#include "OSCAddress.h"
#include "OSCT_ETypes.h"
#include "OSCToolsetLog.h"


template<typename T>
FString UOSCT_Functions::GetEnumString(T EnumValue)
{
	const UEnum* EnumPtr = StaticEnum<T>();
	if (!EnumPtr || !EnumPtr->IsValidLowLevel()) return TEXT("INVALID");
	return EnumPtr->GetNameStringByValue(static_cast<int64>(EnumValue));
}

FString UOSCT_Functions::CleanupAddress(const FString& InAddress)
{
	FString CleanAddress = InAddress;
	CleanAddress.RemoveFromStart(TEXT("/"));
	return CleanAddress;
}

bool UOSCT_Functions::FormatAddress(const FOSCT_Module& Module, FString& OutAddress)
{
	if (Module.Address.IsEmpty()) return false;

	FString RoleStr = GetEnumString(Module.Role).ToUpper();
	FString ModuleStr = GetEnumString(Module.ModuleType).ToUpper();
   
	FString CleanAddress = Module.Address;
	CleanAddress.RemoveFromStart(TEXT("/"));

	// Mutate the struct member directly
	OutAddress = FString::Printf(TEXT("/OSCT/%s/%s%s/%s"), 
	   *RoleStr, 
	   *ModuleStr, 
	   Module.Pack ? TEXT("/p") : TEXT(""), 
	   *CleanAddress);

	return true;
}

void UOSCT_Functions::SendModuleStateUpdate(UOSCClient* Client, const FOSCT_Module& Data, UObject* Context, bool bIsConnecting)
{
	if (!Client || !Context) return;

	FOSCMessage Msg;
	FString Path = bIsConnecting ? TEXT("/OSCT/module_connected") : TEXT("/OSCT/module_disconnected");
	
	// Helper to add "Key%Value" strings quickly
	auto AddKVP = [&](FString Key, FString Value) {
		UOSCManager::AddString(Msg, Key + "%" + Value);
	};
	
	// // FString Formatted;
	// if(!Data.FormattedAddress.IsEmpty())
	// {
	// 	AddKVP(TEXT("FormattedAddress"), Data.FormattedAddress);
	// }
	
	AddKVP(TEXT("Name"), Context->GetName());
	AddKVP(TEXT("Class"), Context->GetClass()->GetName());
	AddKVP(TEXT("Address"), CleanupAddress(Data.Address));
	AddKVP(TEXT("Formatted Address"), Data.FormattedAddress);
	AddKVP(TEXT("Role Type"), UEnum::GetDisplayValueAsText(Data.Role).ToString().ToUpper());
	AddKVP(TEXT("ModuleType"), GetEnumString(Data.ModuleType).ToUpper());
	AddKVP(TEXT("Pack"), Data.Pack ? TEXT("true") : TEXT("false"));

	FOSCAddress OSCAddr = UOSCManager::ConvertStringToOSCAddress(TEXT("/OSCT/module_connected"));
	UOSCManager::SetOSCMessageAddress(Msg, UOSCManager::ConvertStringToOSCAddress(Path));
	Client->SendOSCMessage(Msg);
	UE_LOG(OSCToolset, Log, TEXT("Send Module State Update"));
}
