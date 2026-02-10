// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Engine.h" //Required for GEngine
#include "OSCT_ETypes.h"

#include "OSCT_Functions.generated.h"


/**
 * 
 */
UCLASS()
class OSCTOOLSET_API UOSCT_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	static void DisplayDebug(const FOSCT_Receiver& Module, const FString& Message);
	
public:
	/** Safely converts any Enum to a string without crashing the Editor */
	template<typename T>
	static FString GetEnumString(T EnumValue);

	static EOSCT_ModuleType ConvertSenderTypeToModuleType(EOSCT_SenderType InSender);
	static EOSCT_SenderType ConvertModuleTypeToSenderType(EOSCT_ModuleType InModule);
	static EOSCT_RouteType ConvertModuleTypeToRouteType(EOSCT_ModuleType InModule, const bool bPack);
	static int32 GetComponentLength(EOSCT_RouteType Type);
	
	static FString CleanupAddress(const FString& InAddress);
	
	UFUNCTION(BlueprintCallable, Category="OSCToolset")
	static bool FormatAddress(const EOSCT_Role& Role, const EOSCT_ModuleType& ModuleType, const bool bPack, const FString& Address, FString& OutAddress);
	// static bool FormatAddress(const FOSCT_Module& Module, FString& OutAddress);
	
	UFUNCTION()
	static FOSCMessage CreateStateUpdate(
		UObject* Context,
		const FString& Address,
		const FString& FormattedAddress,
		const EOSCT_Role& Role,
		const EOSCT_ModuleType& ModuleType, 
		const bool Pack,
		const bool bIsConnecting
	);
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Networking")
	static void SendReceiverStateUpdate(UOSCClient* Client, const FOSCT_Receiver& Data, UObject* Context, bool bIsConnecting);
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Networking")
	static void SendSenderStateUpdate(UOSCClient* Client, const FOSCT_Sender& Data, UObject* Context, bool bIsConnecting);

	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Debug")
	static void DebugModule(const FOSCT_Receiver& Module);
	
	//TODO-Pass args instead of a Fstruct.
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Debug")
	static void DebugReceiver(const FOSCT_ReceiverLink& Receiver);	
	
	UFUNCTION(BlueprintCallable, Category = "OSCToolset|Debug")
	static FColor GetModuleDebugColor(const EOSCT_ModuleType& ModuleType);
};
